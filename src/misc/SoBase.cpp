/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

/*!
  \class SoBase SoBase.h Inventor/misc/SoBase.h
  \brief The SoBase class is the top-level superclass for a number
  of class-hierarchies.
  \ingroup general

  SoBase provides the basic interfaces and methods for doing reference
  counting, type identification and import/export. All classes in Coin
  which uses these mechanisms are descendent from this class.
*/

// FIXME: There's a lot of methods in SoBase used to implement VRML
// support which are missing.
//
// UPDATE 20020217 mortene: is this FIXME still correct?

// FIXME: One more thing missing: detect cases where we should
// instantiate SoUnknownEngine instead of SoUnknownNode.

#include "../upgraders/SoUpgrader.h"
#include <Inventor/C/threads/threadsutilp.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/C/tidbitsp.h>
#include <Inventor/SbDict.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/engines/SoEngineOutput.h>
#include <Inventor/engines/SoNodeEngine.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/lists/SoBaseList.h>
#include <Inventor/lists/SoFieldList.h>
#include <Inventor/misc/SoProto.h>
#include <Inventor/misc/SoProtoInstance.h>
#include <Inventor/nodes/SoUnknownNode.h>
#include <Inventor/sensors/SoDataSensor.h>
#include <assert.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

// Note: the following documentation for getTypeId() will also be
// visible for subclasses, so keep it general.
/*!
  \fn SoType SoBase::getTypeId(void) const

  Returns the type identification of an object derived from a class
  inheriting SoBase.  This is used for run-time type checking and
  "downward" casting.

  Usage example:

  \code
  void foo(SoNode * node)
  {
    if (node->getTypeId() == SoFile::getClassTypeId()) {
      SoFile * filenode = (SoFile *)node;  // safe downward cast, knows the type
      /// [then something] ///
    }
    else if (node->getTypeId().isOfType(SoGroup::getClassTypeId())) {
      SoGroup * group = (SoGroup *)node;  // safe downward cast, knows the type
      /// [then something else] ///
    }
  }
  \endcode


  For application programmers wanting to extend the library with new
  nodes, engines, nodekits, draggers or others: this method needs to
  be overridden in \e all subclasses. This is typically done as part
  of setting up the full type system for extension classes, which is
  usually accomplished by using the pre-defined macros available
  through for instance Inventor/nodes/SoSubNode.h (SO_NODE_INIT_CLASS
  and SO_NODE_CONSTRUCTOR for node classes),
  Inventor/engines/SoSubEngine.h (for engine classes) and so on.

  For more information on writing Coin extensions, see the class
  documentation of the toplevel superclasses for the various class
  groups.
*/

// Note: the following documentation for readInstance() will also be
// visible for subclasses, so keep it general.
/*!
  \fn SbBool SoBase::readInstance(SoInput * in, unsigned short flags)

  This method is mainly intended for internal use during file import
  operations.

  It reads a definition of an instance from the input stream \a in.
  The input stream state points to the start of a serialized /
  persistant representation of an instance of this class type.

  \c TRUE or \c FALSE is returned, depending on if the instantiation
  and configuration of the new object of this class type went ok or
  not.  The import process should be robust and handle corrupted input
  streams by returning \c FALSE.

  \a flags is used internally during binary import when reading user
  extension nodes, group nodes or engines.
*/

/*!
  \enum SoBase::BaseFlags
  \COININTERNAL
*/



// Strings and character tokens used in parsing.
static const char OPEN_BRACE = '{';
static const char CLOSE_BRACE = '}';
static const char END_OF_LINE[] = "\n";
static const char DEF_KEYWORD[] = "DEF";
static const char USE_KEYWORD[] = "USE";
static const char NULL_KEYWORD[] = "NULL";
static const char ROUTE_KEYWORD[] = "ROUTE";

static const char PROTO_KEYWORD[] = "PROTO";
static const char EXTERNPROTO_KEYWORD[] = "EXTERNPROTO";

// Reference id if no DEF instance of a node is written yet
static const int REFID_FIRSTWRITE = -1;
// Reference id if we don't need to add a suffix to the node name
static const int REFID_NOSUFFIX = -2;

static SbDict * sobase_auditordict = NULL;
static void * sobase_mutex = NULL;

static void
sobase_auditordict_cb(unsigned long key, void * value)
{
  SoAuditorList * l = (SoAuditorList*) value;
  delete l;
}

static void
sobase_cleanup_auditordict(void)
{
  if (sobase_auditordict) {
    sobase_auditordict->applyToAll(sobase_auditordict_cb);
    delete sobase_auditordict;
    sobase_auditordict = NULL;
  }
}

// Only a small number of SoBase derived objects will under usual
// conditions have designated names, so we use a couple of static
// SbDict objects to keep track of them. Since we avoid storing a
// pointer for each and every object, we'll cut down on a decent
// amount of memory use this way (SoBase should be kept as slim as
// possible, as any dead weight is brought along in a lot of objects).
SbDict * SoBase::name2obj; // maps from char * to SbPList(SoBase)
SbDict * SoBase::obj2name; // maps from SoBase * to char *

SbString * SoBase::refwriteprefix = NULL;

// Don't set value explicitly to SoType::badType(), to avoid a bug in
// Sun CC v4.0. (Bitpattern 0x0000 equals SoType::badType()).
SoType SoBase::classTypeId;

SbBool SoBase::tracerefs = FALSE;
uint32_t SoBase::writecounter = 0;

/**********************************************************************/

//
// If this environment variable is set to 1, we try to preserve
// the original node names as far as possible instead of appending
// a "+<refid>" suffix.
//
static SbBool
dont_mangle_output_names(const SoBase *base)
{
  static int COIN_DONT_MANGLE_OUTPUT_NAMES = -1;

  // Always unmangle node names in VRML1 and VRML2
  if (base->isOfType(SoNode::getClassTypeId()) &&
      (((SoNode *)base)->getNodeType()==SoNode::VRML1 ||
       ((SoNode *)base)->getNodeType()==SoNode::VRML2)) return TRUE;

  if (COIN_DONT_MANGLE_OUTPUT_NAMES < 0) {
    COIN_DONT_MANGLE_OUTPUT_NAMES = 0;
    const char * env = coin_getenv("COIN_DONT_MANGLE_OUTPUT_NAMES");
    if (env) COIN_DONT_MANGLE_OUTPUT_NAMES = atoi(env);
  }
  return COIN_DONT_MANGLE_OUTPUT_NAMES ? TRUE : FALSE;
}

// For counting write references during SoWriteAction traversal, to
// make DEF / USE come out correctly in the output. The hash mapping
// is from SoBase* -> int.
//
// FIXME: this data-structure is not safe to use in a multithreading
// context, with multiple SoWriteAction instances applied in
// parallel. 20020324 mortene.
static SbDict * writerefs = NULL;

static inline int
get_current_writeref(const SoBase * base)
{
  void * val;
  SbBool found = writerefs->find((unsigned long)base, val);
  int refcount = 0;
  if (found) { refcount = (int)((long)val); }
  return refcount;
}

static inline void
set_current_writeref(const SoBase * base, const int rc)
{
  assert(rc >= 0 && "buggy writerefcounter");
  (void)writerefs->enter((unsigned long)base, (void *)rc);
}

/**********************************************************************/

// This can be any "magic" bitpattern of 4 bits which seems unlikely
// to be randomly assigned to a memory byte upon destruction. I chose
// "1101".
//
// The 4 bits allocated for the "alive" bitpattern is used in
// SoBase::ref() to try to detect when the instance has been
// prematurely destructed.
//
// <mortene@sim.no>
#define ALIVE_PATTERN 0xd


/*!
  Constructor. The initial reference count will be set to zero.
 */
SoBase::SoBase(void)
{
  cc_rbptree_init(&this->auditortree);

  this->objdata.referencecount = 0;
  this->objdata.ingraph = FALSE;

  if (writerefs == NULL) {
    writerefs = new SbDict;
    // Since the SbDict design is crap, make sure we at least detect
    // problems on platforms not commonly used internally for testing.
    assert((sizeof(unsigned long) >= sizeof(SoBase *)) && "SbDict overflow");
    assert((sizeof(void *) >= sizeof(int)) && "SbDict overflow");
  }

  // For debugging -- we try to catch dangling references after
  // premature destruction. See the SoBase::assertAlive() method for
  // further doc.
  this->objdata.alive = ALIVE_PATTERN;
}

/*!
  Destructor. There should not be any normal circumstance where you need
  to explicitly delete an object derived from the SoBase class, as the
  reference counting should take care of deallocating unused objects.

  \sa unref(), unrefNoDelete()
 */
SoBase::~SoBase()
{
#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::~SoBase", "%p", this);
#endif // debug

  // Set the 4 bits of bitpattern to anything but the "magic" pattern
  // used to check that we are still alive.
  this->objdata.alive = (~ALIVE_PATTERN) & 0xf;

  if (sobase_auditordict) {
    void * tmp;
    if (sobase_auditordict->find((unsigned long) this, tmp)) {
      delete ((SoAuditorList*) tmp);
      sobase_auditordict->remove((unsigned long) this);
    }
  }
  cc_rbptree_clean(&this->auditortree);
}

//
// callback from auditortree that is used to add sensor
// auditors to the list (closure).
//
static void
sobase_sensor_add_cb(void * auditor, void * type, void * closure)
{
  SbList<SoDataSensor *> * auditingsensors =
    (SbList<SoDataSensor*> *) closure;

  // use a temporary variable, since some compilers can't cast
  // directly from void * to SoNotRec::Type
  uint32_t tmp = (int)((long)type);
  switch ((SoNotRec::Type) tmp) {
  case SoNotRec::SENSOR:
    auditingsensors->append((SoDataSensor *)auditor);
    break;

  case SoNotRec::FIELD:
  case SoNotRec::ENGINE:
  case SoNotRec::CONTAINER:
  case SoNotRec::PARENT:
    // FIXME: should any of these get special treatment? 20000402 mortene.
    break;

  default:
    assert(0 && "Unknown auditor type");
  }
}

/*!
  Cleans up all hanging references to and from this instance, and then
  commits suicide.

  Called automatically when the reference count goes to zero.
*/
void
SoBase::destroy(void)
{
#if COIN_DEBUG && 0 // debug
  SbName dn = this->getName();
  SoType t = this->getTypeId();
  SoDebugError::postInfo("SoBase::destroy", "start -- %p '%s' ('%s')",
                         this, t.getName().getString(), dn.getString());
#endif // debug


#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::destroy",
                           "%p ('%s')",
                           this, this->getTypeId().getName().getString());
  }
#endif // COIN_DEBUG

  // Find all auditors that they need to cut off their link to this
  // object. I believe this is necessary only for sensors.
  SbList<SoDataSensor *> auditingsensors;
  cc_rbptree_traverse(&this->auditortree, (cc_rbptree_traversecb *)sobase_sensor_add_cb, &auditingsensors);

  // Notify sensors that we're dying.
  for (int j = 0; j < auditingsensors.getLength(); j++)
    auditingsensors[j]->dyingReference();

  // Link out instance name from the list of all SoBase instances.
  SbName n = this->getName();
  if (n.getLength()) this->removeName(this, n.getString());

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::destroy", "delete this %p", this);
#endif // debug

  // Sj�lmord.
  delete this;

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::destroy", "done -- %p '%s' ('%s')",
                         this, t.getName().getString(), dn.getString());
#endif // debug
}

/*!
  Sets up initialization for data common to all instances of this
  class, like submitting necessary information to the Coin type
  system.
 */
void
SoBase::initClass(void)
{
  coin_atexit((coin_atexit_f *)SoBase::cleanClass, 0);

  // Avoid multiple attempts at initialization.
  assert(SoBase::classTypeId == SoType::badType());

  SoBase::classTypeId = SoType::createType(SoType::badType(), "Base");

  SoBase::name2obj = new SbDict;
  SoBase::obj2name = new SbDict;
  SoBase::refwriteprefix = new SbString("+");

  CC_MUTEX_CONSTRUCT(sobase_mutex);
}

// Clean up all commonly allocated resources before application
// exit. Only for debugging purposes.
void
SoBase::cleanClass(void)
{
#if COIN_DEBUG
  assert(SoBase::name2obj);
  assert(SoBase::obj2name);

  // Delete the SbPLists in the dictionaries.
  SoBase::name2obj->applyToAll(SoBase::freeLists);

  delete SoBase::name2obj; SoBase::name2obj = NULL;
  delete SoBase::obj2name; SoBase::obj2name = NULL;

  delete writerefs;
  delete SoBase::refwriteprefix;

  CC_MUTEX_DESTRUCT(sobase_mutex);
#endif // COIN_DEBUG
}

/*!
  \COININTERNAL

  There are 4 bits allocated for each SoBase-object for a bitpattern
  that indicates the object is still "alive". The pattern is changed
  when the object is destructed. If this method is then called after
  destruction, an assert will hit.

  This is used internally in Coin (in for instance SoBase::ref()) to
  try to detect when the instance has been prematurely
  destructed. This is a very common mistake to make by application
  programmers (letting the refcount dip to zero before it should, that
  is), so the extra piece of assistance through the accompanying
  assert() in this method to detect dangling references to the object,
  with subsequent memory corruption and mysterious crashes, should be
  a Good Thing.

  \COIN_FUNCTION_EXTENSION

  \since Coin 2.0
*/
void
SoBase::assertAlive(void) const
{
  if (this->objdata.alive != ALIVE_PATTERN) {
    SoDebugError::post("SoBase::assertAlive",
                       "Detected an attempt to access an instance (%p) of an "
                       "SoBase-derived class after it was destructed!  "
                       "This is most likely to be the result of some grave "
                       "programming error in the application / client "
                       "code (or less likely: internal library code), "
                       "causing premature destruction of a reference "
                       "counted object instance. This check was called "
                       "from a dangling reference to it.", this);
    assert(FALSE && "SoBase-object no longer alive!");
  }
}

/*!
  Increase the reference count of the object. This might be necessary
  to do explicitly from user code for certain situations (chiefly to
  avoid premature deletion), but is usually called from other instances
  within the Coin library when objects are made dependent on each other.

  \sa unref(), unrefNoDelete()
 */
void
SoBase::ref(void) const
{
  if (COIN_DEBUG) this->assertAlive();

  // Cast away constness.
  SoBase * base = (SoBase *)this;

#if COIN_DEBUG
  int32_t currentrefcount = base->objdata.referencecount;
#endif // COIN_DEBUG

  CC_MUTEX_LOCK(sobase_mutex);
  base->objdata.referencecount++;
  CC_MUTEX_UNLOCK(sobase_mutex);

#if COIN_DEBUG
  if (base->objdata.referencecount < currentrefcount) {
    SoDebugError::post("SoBase::ref",
                       "%p ('%s') - referencecount overflow!: %d -> %d",
                       this, this->getTypeId().getName().getString(),
                       currentrefcount, this->objdata.referencecount);

    // The reference counter is contained within 27 bits of signed
    // integer, which means it can go up to about ~67 million
    // references. It's hard to imagine that this should be too small,
    // so we don't bother to try to handle overflows any better than
    // this.
    //
    // If we should ever revert this decision, look in Coin-1 for how
    // to handle overflows graciously.
    assert(FALSE && "reference count overflow");
  }
#endif // COIN_DEBUG

#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::ref",
                           "%p ('%s') - referencecount: %d",
                           this, this->getTypeId().getName().getString(),
                           this->objdata.referencecount);
  }
#endif // COIN_DEBUG
}

/*!
  Decrease the reference count of an object. If the reference count
  reaches zero, the object will delete itself. Be careful when
  explicitly calling this method, beware that you usually need to
  match user level calls to ref() with calls to either unref() or
  unrefNoDelete() to avoid memory leaks.

  \sa ref(), unrefNoDelete()
*/
void
SoBase::unref(void) const
{
  if (COIN_DEBUG) this->assertAlive();

  // Cast away constness.
  SoBase * base = (SoBase *)this;
  CC_MUTEX_LOCK(sobase_mutex);
  base->objdata.referencecount--;
  CC_MUTEX_UNLOCK(sobase_mutex);

#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::unref",
                           "%p ('%s') - referencecount: %d",
                           this, this->getTypeId().getName().getString(),
                           this->objdata.referencecount);
  }
  if (this->objdata.referencecount < 0) {
    // Do the debug output in two calls, since the getTypeId() might
    // cause a crash, and then there'd be no output at all with a
    // single SoDebugError::postWarning() call.
    SoDebugError::postWarning("SoBase::unref", "ref count less than zero");
    SoDebugError::postWarning("SoBase::unref", "...for %s instance at %p",
                              this->getTypeId().getName().getString(), this);
  }
#endif // COIN_DEBUG
  if (this->objdata.referencecount == 0) base->destroy();
}

/*!
  Decrease reference count, but do \e not delete ourself if the count
  reaches zero.

  \sa unref()
 */
void
SoBase::unrefNoDelete(void) const
{
  if (COIN_DEBUG) this->assertAlive();

  // Cast away constness.
  SoBase * base = (SoBase *)this;
  base->objdata.referencecount--;
#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::unrefNoDelete",
                           "%p ('%s') - referencecount: %d",
                           this, this->getTypeId().getName().getString(),
                           this->objdata.referencecount);
  }
#endif // COIN_DEBUG
}

/*!
  Returns number of objects referring to this object.
*/
int32_t
SoBase::getRefCount(void) const
{
  return this->objdata.referencecount;
}

/*!
  Force an update, in the sense that all objects connected to this
  will have to re-check the values of their inter-dependent data.

  This is often used as an effective way of manually triggering a
  redraw by application programmers.
 */
void
SoBase::touch(void)
{
  this->startNotify();
}

/*!
  Returns \c TRUE if the type of this object is either of the same
  type or inherited from \a type.
 */
SbBool
SoBase::isOfType(SoType type) const
{
  return this->getTypeId().isDerivedFrom(type);
}

/*!
  This static method returns the SoType object associated with
  objects of this class.
 */
SoType
SoBase::getClassTypeId(void)
{
  return SoBase::classTypeId;
}

/*!
  Returns name of object. If no name has been given to this object,
  the returned SbName will contain the empty string.
 */
SbName
SoBase::getName(void) const
{
  // If this assert fails, SoBase::initClass() has probably not been
  // called.
  assert(SoBase::obj2name);

  void * value;
  CC_MUTEX_LOCK(sobase_mutex);
  SbBool found = SoBase::obj2name->find((unsigned long)this, value);
  CC_MUTEX_UNLOCK(sobase_mutex);
  return found ? SbName((const char *)value) : SbName();
}

/*!
  Set the name of this object.

  Characters that are invalid to use as parts of names for SoBase
  derived objects will be automatically replaced by underscore
  characters. If the name \e starts with an invalid character, the new
  name will be \e preceded by an underscore character.

  \sa getName(), SbName::isBaseNameStartChar(), SbName::isBaseNameChar()
*/
void
SoBase::setName(const SbName & newname)
{
  // remove old name first
  SbName oldName = this->getName();
  if (oldName.getLength()) this->removeName(this, oldName.getString());

  // check for bad characters
  const char * str = newname.getString();
  SbBool isbad = FALSE;

  isbad = (newname.getLength() > 0) && !SbName::isBaseNameStartChar(str[0]);

  int i;
  for (i = 1; i < newname.getLength() && !isbad; i++) {
    isbad = !SbName::isBaseNameChar(str[i]);
  }

  if (isbad) {
    // replace bad characters
    SbString goodstring;

    if (!SbName::isBaseNameStartChar(str[0])) goodstring += '_';

    for (i = 0; i < newname.getLength(); i++) {
      goodstring += SbName::isBaseNameChar(str[i]) ? str[i] : '_';
    }

#if COIN_DEBUG
    SoDebugError::postWarning("SoBase::setName", "Bad characters in "
                              "name '%s'. Replacing with name '%s'",
                              str, goodstring.getString());
#endif // COIN_DEBUG

    this->addName(this, SbName(goodstring.getString()));
  }
  else {
    this->addName(this, newname.getString());
  }
}

/*!
  Adds a name<->object mapping to the global dictionary.
 */
void
SoBase::addName(SoBase * const b, const char * const name)
{
  SbPList * l;
  void * t;
  CC_MUTEX_LOCK(sobase_mutex);
  if (!SoBase::name2obj->find((unsigned long)name, t)) {
    // name not used before, create new list
    l = new SbPList;
    SoBase::name2obj->enter((unsigned long)name, l);
  }
  else {
    // name is used before, find pointer to old list
    l = (SbPList *)t;
  }
  // append this to the list
  l->append(b);

  // set name of object. SbDict::enter() will overwrite old name
  SoBase::obj2name->enter((unsigned long)b, (void *)name);
  CC_MUTEX_UNLOCK(sobase_mutex);
}

/*!
  Removes a name<->object mapping from the global dictionary.
*/
void
SoBase::removeName(SoBase * const b, const char * const name)
{
  CC_MUTEX_LOCK(sobase_mutex);
  void * t;
  SbBool found = SoBase::name2obj->find((unsigned long)name, t);
  assert(found);

  SbPList * list = (SbPList *) t;
  int i = list->find(b);
  assert(i >= 0);
  list->remove(i);

  SoBase::obj2name->remove((unsigned long)b);
  CC_MUTEX_UNLOCK(sobase_mutex);
}

/*!
  This is the method which starts the notification sequence
  after changes.

  At the end of a notification sequence, all "immediate" sensors
  (i.e. sensors set up with a zero priority) are triggered.
*/
void
SoBase::startNotify(void)
{
  SoNotList l;
  SoNotRec rec(this);
  l.append(&rec);
  l.setLastType(SoNotRec::CONTAINER);

  SoDB::startNotify();
  this->notify(&l);
  SoDB::endNotify();
}

// only needed for the callback from cc_rbptree_traverse
typedef struct {
  int cnt;
  int total;
  SoNotList * list;
  SoBase * thisp;
  SbList <void*> notified;
} sobase_notify_data;

//
// Callback from cc_rbptree_traverse().
//
void
SoBase::rbptree_notify_cb(void * auditor, void * type, void * closure)
{
  sobase_notify_data * data = (sobase_notify_data*) closure;
  data->cnt--;

  // gcc will not allow direct cast from void * to SoNotRec::Type
  uint32_t tmptype = (uint32_t)((long)type);

  if (data->notified.find(auditor) < 0) {
    if (data->cnt == 0) {
      data->thisp->doNotify(data->list, auditor, (SoNotRec::Type) tmptype);
    }
    else {
      assert(data->cnt > 0);
      // use a copy of 'l', since the notification list might change
      // when auditors are notified
      SoNotList listcopy(data->list);
      data->thisp->doNotify(&listcopy, auditor, (SoNotRec::Type) tmptype);
    }
  }
}

/*!
  Notifies all auditors for this instance when changes are made.
*/
void
SoBase::notify(SoNotList * l)
{
  if (COIN_DEBUG) this->assertAlive();

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::notify", "base %p, list %p", this, l);
#endif // debug

  sobase_notify_data notdata;
  notdata.cnt = cc_rbptree_size(&this->auditortree);
  notdata.list = l;
  notdata.thisp = this;

  cc_rbptree_traverse(&this->auditortree, (cc_rbptree_traversecb *)SoBase::rbptree_notify_cb, &notdata);
  assert(notdata.cnt == 0);
}

/*!
  Add an auditor to notify whenever the object changes in any significant
  way.

  \sa removeAuditor()
 */
void
SoBase::addAuditor(void * const auditor, const SoNotRec::Type type)
{
  cc_rbptree_insert(&this->auditortree, auditor, (void*) type);
}

/*!
  Remove an auditor from the list. \a auditor will then no longer be
  notified whenever any updates are made to this object.

  \sa addAuditor()
*/
void
SoBase::removeAuditor(void * const auditor, const SoNotRec::Type type)
{
  cc_rbptree_remove(&this->auditortree, auditor);
}


static void
sobase_audlist_add(void * pointer, void * type, void * closure)
{
  SoAuditorList * list = (SoAuditorList*) closure;
  uint32_t tmp = (uint32_t)((long)type);


  list->append(pointer, (SoNotRec::Type) tmp);
}

/*!
  Returns list of objects auditing this object.

  \sa addAuditor(), removeAuditor()
*/
const SoAuditorList &
SoBase::getAuditors(void) const
{
  CC_MUTEX_LOCK(sobase_mutex);

  if (sobase_auditordict == NULL) {
    sobase_auditordict = new SbDict();
    coin_atexit((coin_atexit_f*)sobase_cleanup_auditordict, 0);
  }

  SoAuditorList * list = NULL;
  void * tmp;
  if (sobase_auditordict->find((unsigned long) this, tmp)) {
    list = (SoAuditorList*) tmp;
    // empty list before copying in new values
    for (int i = 0; i < list->getLength(); i++) {
      list->remove(i);
    }
  }
  else {
    list = new SoAuditorList;
    sobase_auditordict->enter((unsigned long) this, (void*) list);
  }
  cc_rbptree_traverse(&this->auditortree, (cc_rbptree_traversecb*)sobase_audlist_add, (void*) list);

  CC_MUTEX_UNLOCK(sobase_mutex);

  return *list;
}

/*!
  This method is used during the first write pass of a write action to
  count the number of references to this object in the scene graph.
*/
void
SoBase::addWriteReference(SoOutput * out, SbBool isfromfield)
{
  assert(out->getStage() == SoOutput::COUNT_REFS);

  int refcount = get_current_writeref(this);

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::addWriteReference",
                         "%p/%s: %d -> %d",
                         this, this->getTypeId().getName().getString(),
                         refcount, refcount + 1);
#endif // debug

  refcount++;

  if (!isfromfield) this->objdata.ingraph = TRUE;

  set_current_writeref(this, refcount);
}

/*!
  Returns \c TRUE if this object should be written out during a write action.
  Will return \c FALSE if no references to this object has been made in the
  scene graph.

  Note that connections from the fields of fieldcontainer objects is not
  alone a valid reason for writing out the object -- there must also be at
  least one reference directly from another SoBase (like a parent/child
  relationship, for instance).

  This method will return a valid result only during the second pass of
  write actions.
*/
SbBool
SoBase::shouldWrite(void)
{
  return (this->objdata.ingraph == TRUE);
}

/*!
  \COININTERNAL
*/
void
SoBase::incrementCurrentWriteCounter(void)
{
  SoBase::writecounter++;
}

/*!
  \COININTERNAL
*/
void
SoBase::decrementCurrentWriteCounter(void)
{
  SoBase::writecounter--;
}

/*!
  Returns the object of \a type, or derived from \a type, registered
  under \a name. If several has been registered under the same name
  with the same type, returns the \e last one which was registered.

  If no object of a valid type or subtype has been registered with the
  given name, returns \c NULL.
*/
SoBase *
SoBase::getNamedBase(const SbName & name, SoType type)
{
  CC_MUTEX_LOCK(sobase_mutex);
  void * t;
  if (SoBase::name2obj->find((unsigned long)((const char *)name), t)) {
    SbPList * l = (SbPList *)t;
    if (l->getLength()) {
      SoBase * b = (SoBase *)((*l)[l->getLength() - 1]);
      if (b->isOfType(type)) {
        CC_MUTEX_UNLOCK(sobase_mutex);
        return b;
      }
    }
  }
  CC_MUTEX_UNLOCK(sobase_mutex);
  return NULL;
}

/*!
  Returns the number of objects of type \a type, or derived from \a type,
  registered under \a name.

  All matches will also be appended to \a baselist.
 */
int
SoBase::getNamedBases(const SbName & name, SoBaseList & baselist, SoType type)
{
  CC_MUTEX_LOCK(sobase_mutex);

  int matches = 0;

  void * t;
  if (SoBase::name2obj->find((unsigned long)((const char *)name), t)) {
    SbPList * l = (SbPList *)t;
    for (int i=0; i < l->getLength(); i++) {
      SoBase * b = (SoBase *)((*l)[i]);
      if (b->isOfType(type)) {
        baselist.append(b);
        matches++;
      }
    }
  }
  CC_MUTEX_UNLOCK(sobase_mutex);

  return matches;
}

/*!
  Read next SoBase derived instance from the \a in stream, check that
  it is derived from \a expectedtype and place a pointer to the newly
  allocated instance in \a base.

  \c FALSE is returned on read errors, mismatch with the \a
  expectedtype, or if there are attempts at referencing (through the
  \c USE keyword) unknown instances.

  If we return \c TRUE with \a base equal to \c NULL, three things
  might have happened:

  1. End-of-file. Use SoInput::eof() after calling this method to
  detect end-of-file conditions.

  2. \a in didn't have a valid identifier name at the stream for us to
  read. This happens either in the case of errors, or when all child
  nodes of a group has been read. Check if the next character in the
  stream is a '}' to detect the latter case.

  3. A child was given as the \c NULL keyword. This can happen when
  reading the contents of SoSFNode or SoMFNode fields.

  If \c TRUE is returned and \a base is not \c NULL upon return, the
  instance was allocated and initialized according to what was read
  from the \a in stream.
*/
SbBool
SoBase::read(SoInput * in, SoBase *& base, SoType expectedtype)
{
  assert(expectedtype != SoType::badType());
  base = NULL;

  SbName name;
  SbBool result = in->read(name, TRUE);

#if COIN_DEBUG && 0 // debug
  // This debug statement is extremely useful when debugging the
  // import code, so keep it around.
  SoDebugError::postInfo("SoBase::read", "name: '%s'", name.getString());
#endif // debug

  //.read all (vrml97) routes
  if (in->isFileVRML2()) {
    while (result && name == ROUTE_KEYWORD) {
      result = SoBase::readRoute(in);
      // read next ROUTE keyword
      if (result ) result = in->read(name, TRUE);
      else return FALSE; // error while reading ROUTE
    }
  }

  // The SoInput stream does not start with a valid base name. Return
  // TRUE with base==NULL.
  if (!result) return TRUE;

  if (name == USE_KEYWORD) result = SoBase::readReference(in, base);
  else if (name == NULL_KEYWORD) return TRUE;
  else result = SoBase::readBase(in, name, base);

  // Check type correctness.
  if (result) {
    assert(base);

    SoType type = base->getTypeId();
    assert(type != SoType::badType());

    if (!type.isDerivedFrom(expectedtype)) {
      SoReadError::post(in, "Type '%s' is not derived from '%s'",
                        type.getName().getString(),
                        expectedtype.getName().getString());
      result = FALSE;
    }
  }

  // Make sure we don't leak memory.
  if (!result && base && (name != USE_KEYWORD)) {
    base->ref();
    base->unref();
  }

#if COIN_DEBUG && 0 // debug
  if (result) SoDebugError::postInfo("SoBase::read", "baseptr: %p", base);
#endif // debug

  return result;
}

/*!
  Referenced instances of SoBase are written as "DEF NamePrefixNumber" when
  exported. "Name" is the name of the base instance from setName(), "Prefix"
  is common for all objects and can be set by this method (default is "+"),
  and "Number" is a unique id which is necessary if multiple objects have
  the same name.

  If you want the prefix to be something else than "+", use this method.
 */
void
SoBase::setInstancePrefix(const SbString & c)
{
  (*SoBase::refwriteprefix) = c;
}

/*!
  Set to \c TRUE to activate debugging of reference counting, which
  could aid in finding hard to track down problems with accesses to
  freed memory or memory leaks. Note: this will produce lots of
  debug information in any "normal" running system, so use sensibly.

  The reference tracing functionality will be disabled in "release
  versions" of the Coin library.
 */
void
SoBase::setTraceRefs(SbBool trace)
{
  SoBase::tracerefs = trace;
}

/*!
  Return the status of the reference tracing flag.

  \sa setTraceRefs()
 */
SbBool
SoBase::getTraceRefs(void)
{
  return SoBase::tracerefs;
}

/*!
  Returns \c TRUE if this object will be written more than once upon
  export. Note that the result from this method is only valid during the
  second pass of a write action (and partly during the COUNT_REFS pass).
 */
SbBool
SoBase::hasMultipleWriteRefs(void) const
{
  return get_current_writeref(this) > 1;
}

// FIXME: temporary bug-workaround needed to test if we are exporting
// a VRML97 or an Inventor file. Implementation in SoOutput.cpp.
// pederb, 2003-03-18
extern SbString SoOutput_getHeaderString(const SoOutputP * out);

/*!
  Write out the header of any SoBase derived object. The header consists
  of the \c DEF keyword and the object name (if the object has a name,
  otherwise these will be skipped), the class name and a left bracket.

  Alternatively, the object representation may be made up of just the
  \c USE keyword plus the object name, if this is the second or subsequent
  reference written to the file.

  If the object has been completed just by writing the header (which will
  be the case if we're writing multiple references of an object),
  we return \c TRUE, otherwise \c FALSE.

  If we return \c FALSE (i.e. there's more to write), we will
  increment the indentation level.

  \sa writeFooter(), SoOutput::indent()
 */
SbBool
SoBase::writeHeader(SoOutput * out, SbBool isgroup, SbBool isengine) const
{
  if (!out->isBinary()) {
    out->write(END_OF_LINE);
    out->indent();
  }

  SbName name = this->getName();
  int refid = out->findReference(this);
  SbBool firstwrite = refid == REFID_FIRSTWRITE;
  SbBool multiref = this->hasMultipleWriteRefs();

  // Find what node name to write
  SbString writename;
  if (dont_mangle_output_names(this)) {
    //
    // Try to keep the original node names as far as possible.
    // Weaknesses (FIXME kintel 20020429):
    //  o We should try to reuse refid's as well.
    //  o We should try to let "important" (=toplevel?) nodes
    //    keep their original node names before some subnode "captures" it.
    //

    /* Code example. The correct output file is shown below

       #include <Inventor/SoDB.h>
       #include <Inventor/SoInput.h>
       #include <Inventor/SoOutput.h>
       #include <Inventor/actions/SoWriteAction.h>
       #include <Inventor/nodes/SoSeparator.h>

       void main(int argc, char *argv[])
       {
       SoDB::init();

       SoSeparator *root = new SoSeparator;
       root->ref();
       root->setName("root");

       SoSeparator *n0 = new SoSeparator;
       SoSeparator *a0 = new SoSeparator;
       SoSeparator *a1 = new SoSeparator;
       SoSeparator *a2 = new SoSeparator;
       SoSeparator *a3 = new SoSeparator;
       SoSeparator *b0 = new SoSeparator;
       SoSeparator *b1 = new SoSeparator;
       SoSeparator *b2 = new SoSeparator;
       SoSeparator *b3 = new SoSeparator;
       SoSeparator *b4 = new SoSeparator;
       SoSeparator *c0 = new SoSeparator;

       a2->setName(SbName("MyName"));
       b0->setName(SbName("MyName"));
       b1->setName(SbName("MyName"));
       b2->setName(SbName("MyName"));
       b4->setName(SbName("MyName"));
       c0->setName(SbName("MyName"));

       root->addChild(n0);
       root->addChild(n0);
       root->addChild(a0);
       a0->addChild(b0);
       a0->addChild(b1);
       root->addChild(b0);
       root->addChild(a1);
       a1->addChild(b2);
       a1->addChild(b1);
       root->addChild(b1);
       root->addChild(a2);
       root->addChild(a2);
       root->addChild(a3);
       a3->addChild(b3);
       b3->addChild(c0);
       b3->addChild(c0);
       a3->addChild(b4);
       a3->addChild(a2);

       SoOutput out;
       out.openFile("out.wrl");
       out.setHeaderString(SbString("#VRML V1.0 ascii"));
       SoWriteAction wra(&out);
       wra.apply(root);
       out.closeFile();

       root->unref();
       }

       Output file:

       #VRML V1.0 ascii

       DEF root Separator {
         DEF +0 Separator {
         }
         USE +0
         Separator {
           DEF MyName Separator {
           }
           DEF MyName+1 Separator {
           }
         }
         USE MyName
         Separator {
           DEF MyName Separator {
           }
           USE MyName+1
         }
         USE MyName+1
         DEF MyName Separator {
         }
         USE MyName
         Separator {
           Separator {
             DEF MyName+2 Separator {
             }
             USE MyName+2
           }
           DEF MyName+3 Separator {
           }
           USE MyName
         }
       }
    */

    if (!firstwrite) {
      writename = name.getString();
      // We have used a suffix when DEF'ing the node
      if (refid != REFID_NOSUFFIX) {
        writename += SoBase::refwriteprefix->getString();
        writename.addIntString(refid);
      }
      // Detects last USE of a node, enables reuse of DEF's
      if (!multiref) out->removeDEFNode(SbName(writename));
    }
    else {
      SbBool found = out->lookupDEFNode(name);
      writename = name.getString();
      if (!found && (!multiref || name.getLength() > 0)) {
        // We can use the node without a suffix
        if (multiref) out->addDEFNode(name);
        out->setReference(this, REFID_NOSUFFIX);
      }
      else {
        // Node name is already DEF'ed or an unnamed multiref => use a suffix.
        writename += SoBase::refwriteprefix->getString();
        writename.addIntString(out->addReference(this));
        out->addDEFNode(SbName(writename));
      }
    }
  }
  else { // Default OIV behavior
    if (multiref && firstwrite) refid = out->addReference(this);
    if (!firstwrite) {
      writename = name.getString();
      writename += SoBase::refwriteprefix->getString();
      writename.addIntString(refid);
    }
    else {
      writename = name.getString();
      if (multiref) {
        writename += SoBase::refwriteprefix->getString();
        writename.addIntString(refid);
      }
    }
  }

  // Write the node
  if (!firstwrite) {
    out->write(USE_KEYWORD);
    if (!out->isBinary()) out->write(' ');
    out->write(writename.getString());
  }
  else {
    if (name.getLength() || multiref) {
      out->write(DEF_KEYWORD);
      if (!out->isBinary()) out->write(' ');

      out->write(writename.getString());
      if (!out->isBinary()) out->write(' ');
    }

    if (this->isOfType(SoNode::getClassTypeId()) &&
        ((SoNode*)this)->getNodeType() == SoNode::VRML2) {
      SbString nodename(this->getFileFormatName());
      if (nodename.getLength() > 4) {
        SbString vrml = nodename.getSubString(0, 3);
        // FIXME: using a temporary workaround to test if we're
        // exporting a VRML97 file. pederb, 2003-03-18
        if ((vrml == "VRML") && 
            (SoOutput_getHeaderString(out->pimpl) == "#VRML V2.0 utf8")) {
          SbString substring = nodename.getSubString(4);
          out->write(substring.getString());
        }
        else {
          out->write(nodename.getString());
        }
      }
      else {
        out->write(nodename.getString());
      }
    }
    else {
      out->write(this->getFileFormatName());
    }
    if (out->isBinary()) {
      uint32_t flags = 0x0;
      if (isgroup) flags |= SoBase::IS_GROUP;
      if (isengine) flags |= SoBase::IS_ENGINE;
      out->write(flags);
    }
    else {
      out->write(" {");
      out->write(END_OF_LINE);
      out->incrementIndent();
    }
  }

  int refcount = get_current_writeref(this);

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::writeHeader",
                         "%p/%s: %d -> %d",
                         this,
                         this->getTypeId().getName().getString(),
                         refcount, refcount - 1);
#endif // debug

  SoBase * thisp = (SoBase *)this;
  refcount--;

  if (refcount == 0) {
    // Make ready for next inital write action pass by resetting data.
    thisp->objdata.ingraph = FALSE;
    SbBool found = writerefs->remove((unsigned long)this);
    assert(found && "writeref hash in trouble");

    // Ouch. Does this to avoid having two subsequent write actions on
    // the same SoOutput to write "USE ..." when it should write a
    // full node/subgraph specification on the second run.  -mortene.
    //
    // FIXME: accessing out->removeSoBase2IdRef() directly takes a
    // "friend SoBase" in the SoOutput class definition. Should fix
    // with proper design for Coin-2. 20020426 mortene
    if (out->findReference(this) != REFID_FIRSTWRITE)
      out->removeSoBase2IdRef(this);
  }
  else {
    set_current_writeref(this, refcount);
  }

  // Don't need to write out the rest if we are writing anything but
  // the first instance.
  return (firstwrite == FALSE);
}

/*!
  This method will terminate the block representing an SoBase derived
  object.
 */
void
SoBase::writeFooter(SoOutput * out) const
{
  if (!out->isBinary()) {
    out->decrementIndent();
    out->indent();
    out->write('}');
  }
}

/*!
  Returns the class name this object should be written under.  Default
  string returned is the name of the class from the type system.

  User extensions nodes and engines override this method to return the
  name of the extension (instead of "UnknownNode" or "UnknownEngine").
 */
const char *
SoBase::getFileFormatName(void) const
{
  return this->getTypeId().getName().getString();
}

/*!
  \COININTERNAL
*/
uint32_t
SoBase::getCurrentWriteCounter(void)
{
  return SoBase::writecounter;
}

// Used to free the SbPLists in the name<->object dict.
void
SoBase::freeLists(unsigned long, void * value)
{
  delete (SbPList *)value;
}

// Reads the name of a reference after a "USE" keyword and finds the
// ptr to the object which is being referenced.
SbBool
SoBase::readReference(SoInput * in, SoBase *& base)
{
  SbName refname;
  if (!in->read(refname, FALSE)) {
    SoReadError::post(in, "Premature end of file after \"%s\"", USE_KEYWORD);
    return FALSE;
  }

  // This is a pretty ugly hack to handle cases where the USE ref name
  // is immediately followed by a "." and a fieldname, as can occur
  // when reading field-to-field connections.
  //
  // A proper fix would have to clean up the small mess of how the
  // SoInput::read(SbName name, SbBool validident) method is
  // implemented and used.
  //
  // 20000129 mortene.
  if (!in->isBinary()) {
    const char * refptr = refname.getString();
    const char * dotptr = strrchr(refptr, '.');
    if (dotptr && dotptr != refptr) {
      SbString s = refname.getString();
      refname = s.getSubString(0, (dotptr - refptr - 1));
      in->putBack(dotptr);
    }
  }

  if ((base = in->findReference(refname)) == NULL) {
    SoReadError::post(in, "Unknown reference \"%s\"", refname.getString());
    return FALSE;
  }

  // when referencing an SoProtoInstance, we need to return the proto
  // instance's root node, not the actual proto instance node.
  if (base->isOfType(SoProtoInstance::getClassTypeId())) {
    base = ((SoProtoInstance*) base)->getRootNode();
  }

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::readReference",
                         "USE: '%s'", refname.getString());
#endif // debug

  return TRUE;
}

// Read the SoBase instance.
SbBool
SoBase::readBase(SoInput * in, SbName & classname, SoBase *& base)
{
#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::readBase", "classname: '%s'",
                         classname.getString());
#endif // debug

  SbBool ret = TRUE;
  base = NULL;

  SbName refname;

  if (in->isFileVRML2()) {
    if (classname == PROTO_KEYWORD ||
        classname == EXTERNPROTO_KEYWORD) { // special case to handle [EXTERN]PROTO definitions
      SoProto * proto = new SoProto(classname == EXTERNPROTO_KEYWORD);
      proto->ref();
      ret = proto->readInstance(in, 0);
      if (ret) {
        proto->unrefNoDelete();
        in->addProto(proto);
      }
      else {
        proto->unref();
        return FALSE;
      }
      base = proto;
      return TRUE;
    }
  }

  if (classname == DEF_KEYWORD) {
    if (!in->read(refname, FALSE) || !in->read(classname, TRUE)) {
      SoReadError::post(in, "Premature end of file after %s", DEF_KEYWORD);
      ret = FALSE;
    }

    if (!refname) {
      SoReadError::post(in, "No name given after %s", DEF_KEYWORD);
      ret = FALSE;
    }

    if (!classname) {
      SoReadError::post(in, "Invalid definition of %s", refname.getString());
      ret = FALSE;
    }
  }

  if (ret) {
    SbBool gotchar = FALSE; // Unnecessary, but kills a compiler warning.
    char c;
    if (!in->isBinary() && (!(gotchar = in->read(c)) || c != OPEN_BRACE)) {
      if (gotchar)
        SoReadError::post(in, "Expected '%c'; got '%c'", OPEN_BRACE, c);
      else
        SoReadError::post(in, "Expected '%c'; got EOF", OPEN_BRACE);
      ret = FALSE;
    }
    else {
      ret = SoBase::readBaseInstance(in, classname, refname, base);

      if (ret && !in->isBinary()) {
        if (!(gotchar = in->read(c)) || c != CLOSE_BRACE) {
          if (gotchar)
            SoReadError::post(in, "Expected '%c'; got '%c'", CLOSE_BRACE, c);
          else
            SoReadError::post(in, "Expected '%c'; got EOF", CLOSE_BRACE);
          ret = FALSE;
        }
      }
    }
  }

  return ret;
}

// Read the SoBase instance.
SbBool
SoBase::readBaseInstance(SoInput * in, const SbName & classname,
                         const SbName & refname, SoBase *& base)
{
  SbBool retval = TRUE;
  SbBool needupgrade = FALSE;

  // first, try creating an upgradable node, based on the version of
  // the input file.
  base = SoUpgrader::tryCreateNode(classname, in->getIVVersion());
  if (base) {
    // we need to upgrade the node after reading it
    needupgrade = TRUE;
  }
  else {
    // create normal Coin node
    base = SoBase::createInstance(in, classname);
  }

  if (base) {
    if (!(!refname)) {
      // Set up new entry in reference hash -- with full name.
      in->addReference(refname, base);

      // Remove reference counter suffix, if any (i.e. "goldsphere+2"
      // becomes "goldsphere").
      SbString instancename = refname.getString();
      const char * strp = instancename.getString();
      const char * occ = strstr(strp, SoBase::refwriteprefix->getString());

      if (occ != strp) { // They will be equal if the name is only a refcount.
        if (occ) instancename = instancename.getSubString(0, occ - strp - 1);
        // Set name identifier for newly created SoBase instance.
        base->setName(instancename);
      }
    }

    // The "flags" argument to readInstance is only checked during
    // import from binary format files.
    unsigned short flags = 0;
    if (in->isBinary()) {
      if (in->getIVVersion() > 2.0f) retval = in->read(flags);
    }

    if (retval) retval = base->readInstance(in, flags);

    if (!retval) {
      if (!(!refname)) in->removeReference(refname);
      base->ref();
      base->unref();
      base = NULL;
    }
    else if (needupgrade) {
      SoBase * oldbase = base;
      oldbase->ref();
      base = SoUpgrader::createUpgrade(oldbase);
      assert(base && "should never happen (since needupgrade == TRUE)");
      oldbase->unref();
    }
  }
  else {
    retval = FALSE;
  }

  if (retval && base->isOfType(SoProtoInstance::getClassTypeId())) {
    base = ((SoProtoInstance*) base)->getRootNode();
  }

  return retval;
}

// Create a new instance of the "classname" type.
SoBase *
SoBase::createInstance(SoInput * in, const SbName & classname)
{
  SoType type = SoType::badType();
  if (in->isFileVRML2()) {
    SbString newname;
    newname.sprintf("VRML%s", classname.getString());
    type = SoType::fromName(SbName(newname.getString()));
#if COIN_DEBUG && 0 // debug
    if (type != SoType::badType()) {
      SoDebugError::postInfo("SoBase::createInstance",
                             "Created VRML V2.0 type: %s",
                             type.getName().getString());
    }
#endif // debug
  }

  SoProto * proto = SoProto::findProto(classname);
  if (proto) return proto->createProtoInstance();

  if (type == SoType::badType())
    type = SoType::fromName(classname);

  SoBase * instance = NULL;

  if (type == SoType::badType()) {
    // Default to SoUnknownNode for now.. FIXME: what if we're dealing
    // with an unknown engine? 20000105 mortene.
    SoUnknownNode * unknownnode = new SoUnknownNode;
    unknownnode->setNodeClassName(classname);
    instance = unknownnode;
#if COIN_DEBUG && 0 // debug
    SoDebugError::postInfo("SoBase::createInstance",
                           "created SoUnknownNode for '%s'",
                           classname.getString());
#endif // debug
  }
  else if (!type.canCreateInstance()) {
    SoReadError::post(in, "Class \"%s\" is abstract", classname.getString());
  }
  else {
    instance = (SoBase *)type.createInstance();
  }

  return instance;
}

// Hmm.
void
SoBase::flushInput(SoInput * in)
{
#if 0 // FIXME: obsoleted, see comment at the end of SoBase::readBase(). 20020531 mortene.
  assert(FALSE);
#else // obsoleted
  assert(!in->isBinary());

  int nestlevel = 1;
  char c;

  while (nestlevel > 0 && in->read(c)) {
    if (c == CLOSE_BRACE) nestlevel--;
    else if (c == OPEN_BRACE) nestlevel++;
  }
#endif // obsoleted
}

//
// helper function for connectRoute(). First test the actual fieldname,
// then set set_<fieldname>, then <fieldname>_changed.
//
static SoField *
find_field(SoNode * node, const SbName & fieldname)
{
  SoField * field = node->getField(fieldname);

  if (!field) {
    if (strncmp(fieldname.getString(), "set_", 4) == 0) {
      SbName newname = fieldname.getString() + 4;
      field = node->getField(newname);
    }
    else {
      SbString str = fieldname.getString();
      int len = str.getLength();
      const char CHANGED[] = "_changed";
      const int changedsize = sizeof(CHANGED) - 1;

      if (len > changedsize && strcmp(str.getString()+len-changedsize,
                                      CHANGED) == 0) {
        SbString substr = str.getSubString(0, len-(changedsize+1));
        SbName newname = substr.getString();
        field = node->getField(newname);
      }
    }
  }
  return field;
}

/*!
  Connect a route from the node named \a fromnodename's field \a
  fromfieldname to the node named \a tonodename's field \a
  tofieldname. This method will consider the fields types (event in,
  event out, etc) when connecting.

  \COIN_FUNCTION_EXTENSION

  \since Coin 2.0
*/
SbBool
SoBase::connectRoute(SoInput * in,
                     const SbName & fromnodename, const SbName & fromfieldname,
                     const SbName & tonodename, const SbName & tofieldname)
{
  SoNode * fromnode = SoNode::getByName(fromnodename);
  SoNode * tonode = SoNode::getByName(tonodename);
  if (fromnode && tonode) {
    SoField * from = find_field(fromnode, fromfieldname);
    SoField * to = find_field(tonode, tofieldname);
    SoEngineOutput * output = NULL;
    if (from == NULL && fromnode->isOfType(SoNodeEngine::getClassTypeId())) {
      output = ((SoNodeEngine*) fromnode)->getOutput(fromfieldname);
    }

    if (to && (from || output)) {
      SbBool notnotify = FALSE;
      SbBool append = FALSE;
      if (output || from->getFieldType() == SoField::EVENTOUT_FIELD) {
        notnotify = TRUE;
      }
      if (to->getFieldType() == SoField::EVENTIN_FIELD) append = TRUE;

      // Check if we're already connected.
      SoFieldList fl;
      if (from) from->getForwardConnections(fl);
      else output->getForwardConnections(fl);
      int idx = fl.find(to);
      if (idx != -1) {
        SoReadError::post(in,
                          "Tried to connect a ROUTE multiple times "
                          "(from %s.%s to %s.%s)",
                          fromnodename.getString(), fromfieldname.getString(),
                          tonodename.getString(), tofieldname.getString());
        return FALSE;
      }

      // Check that there exists a field converter, if one is needed.
      SoType totype = to->getTypeId();
      SoType fromtype = from ? from->getTypeId() : output->getConnectionType();
      if (totype != fromtype) {
        SoType convtype = SoDB::getConverter(fromtype, totype);
        if (convtype == SoType::badType()) {
          SoReadError::post(in,
                            "Tried to connect a ROUTE between entities "
                            "that can not be connected (due to lack of "
                            "field type converter): %s.%s is of type "
                            "%s, and %s.%s is of type %s",
                            fromnodename.getString(), fromfieldname.getString(),
                            fromtype.getName().getString(),
                            tonodename.getString(), tofieldname.getString(),
                            totype.getName().getString());
          return FALSE;
        }
      }

      SbBool ok;
      if (from) ok = to->connectFrom(from, notnotify, append);
      else ok = to->connectFrom(output, notnotify, append);
      // Both known possible failure points are caught above.
      assert(ok && "unexpected connection error");

      return TRUE;
    }
  }
  return FALSE;
}


/*!
  \COININTERNAL

  Reads a (VRML97) ROUTE. We decided to also add support for routes in
  Coin, as a generic feature, since we think it is nicer than setting
  up field connections inside the nodes.

*/
SbBool
SoBase::readRoute(SoInput * in)
{
  SbString fromstring, tostring;

  SbName fromnodename;
  SbName fromfieldname;
  SbName toname;
  SbName tonodename;
  SbName tofieldname;
  SbBool ok;

  ok =
    in->read(fromstring) &&
    in->read(toname) &&
    in->read(tostring);

  if (ok) ok = (toname == SbName("TO"));

  if (ok) {
    ok = FALSE;

    // parse from-string
    char * str1 = (char*) fromstring.getString();
    char * str2 = str1 ? (char*) strchr(str1, '.') : NULL;
    if (str1 && str2) {
      *str2++ = 0;

      // now parse to-string
      fromnodename = str1;
      fromfieldname = str2;
      str1 = (char*) tostring.getString();
      str2 = str1 ? strchr(str1, '.') : NULL;
      if (str1 && str2) {
        *str2++ = 0;
        tonodename = str1;
        tofieldname = str2;

        ok = TRUE;
      }
    }
  }

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::readRoute",
                         "%s.%s %s %s.%s",
                         fromnodename.getString(),
                         fromfieldname.getString(),
                         toname.getString(),
                         tonodename.getString(),
                         tofieldname.getString());
#endif // debug

  if (!ok) SoReadError::post(in, "Error parsing ROUTE keyword");
  else {
    SoProto * proto = in->getCurrentProto();
    if (proto) {
      proto->addRoute(fromnodename, fromfieldname, tonodename, tofieldname);
    }
    else {
      SoNode * fromnode = SoNode::getByName(fromnodename);
      SoNode * tonode = SoNode::getByName(tonodename);

      if (!fromnode || !tonode) {
        SoReadError::post(in,
                          "Unable to create ROUTE from %s.%s to %s.%s. "
                          "Delaying.",
                          fromnodename.getString(), fromfieldname.getString(),
                          tonodename.getString(), tofieldname.getString());
        in->addRoute(fromnodename, fromfieldname, tonodename, tofieldname);
      }

      (void)SoBase::connectRoute(in, fromnodename, fromfieldname,
                                 tonodename, tofieldname);
    }
  }
  return ok;
}

//
// private method that sends a notify to auditor based on type
//
void
SoBase::doNotify(SoNotList * l, const void * auditor, const SoNotRec::Type type)
{
  l->setLastType(type);
  switch (type) {
  case SoNotRec::CONTAINER:
  case SoNotRec::PARENT:
    {
      SoFieldContainer * obj = (SoFieldContainer *)auditor;
      obj->notify(l);
    }
    break;

  case SoNotRec::SENSOR:
    {
      SoDataSensor * obj = (SoDataSensor *)auditor;
#if COIN_DEBUG && 0 // debug
      SoDebugError::postInfo("SoAuditorList::notify",
                             "notify and schedule sensor: %p", obj);
#endif // debug
      // don't schedule the sensor here. The sensor instance will do
      // that in notify() (it might also choose _not_ to schedule),
      obj->notify(l);
    }
    break;

  case SoNotRec::FIELD:
  case SoNotRec::ENGINE:
    {
      // We used to check whether or not the fields was already
      // dirty before we transmitted the notification
      // message. This is _not_ correct (the dirty flag is
      // conceptually only relevant for whether or not to do
      // re-evaluation), so don't try to "optimize" the
      // notification mechanism by re-introducing that "feature".
      // :^/
      ((SoField *)auditor)->notify(l);
    }
    break;

  default:
    assert(0 && "Unknown auditor type");
  }
}
