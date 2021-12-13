// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME REXppRootEventDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "include/REXppRootEvent.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_REXppRootEvent(void *p = 0);
   static void *newArray_REXppRootEvent(Long_t size, void *p);
   static void delete_REXppRootEvent(void *p);
   static void deleteArray_REXppRootEvent(void *p);
   static void destruct_REXppRootEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::REXppRootEvent*)
   {
      ::REXppRootEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::REXppRootEvent >(0);
      static ::ROOT::TGenericClassInfo 
         instance("REXppRootEvent", ::REXppRootEvent::Class_Version(), "REXppRootEvent.h", 6,
                  typeid(::REXppRootEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::REXppRootEvent::Dictionary, isa_proxy, 4,
                  sizeof(::REXppRootEvent) );
      instance.SetNew(&new_REXppRootEvent);
      instance.SetNewArray(&newArray_REXppRootEvent);
      instance.SetDelete(&delete_REXppRootEvent);
      instance.SetDeleteArray(&deleteArray_REXppRootEvent);
      instance.SetDestructor(&destruct_REXppRootEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::REXppRootEvent*)
   {
      return GenerateInitInstanceLocal((::REXppRootEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::REXppRootEvent*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr REXppRootEvent::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *REXppRootEvent::Class_Name()
{
   return "REXppRootEvent";
}

//______________________________________________________________________________
const char *REXppRootEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::REXppRootEvent*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int REXppRootEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::REXppRootEvent*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *REXppRootEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::REXppRootEvent*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *REXppRootEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::REXppRootEvent*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void REXppRootEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class REXppRootEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(REXppRootEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(REXppRootEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_REXppRootEvent(void *p) {
      return  p ? new(p) ::REXppRootEvent : new ::REXppRootEvent;
   }
   static void *newArray_REXppRootEvent(Long_t nElements, void *p) {
      return p ? new(p) ::REXppRootEvent[nElements] : new ::REXppRootEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_REXppRootEvent(void *p) {
      delete ((::REXppRootEvent*)p);
   }
   static void deleteArray_REXppRootEvent(void *p) {
      delete [] ((::REXppRootEvent*)p);
   }
   static void destruct_REXppRootEvent(void *p) {
      typedef ::REXppRootEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::REXppRootEvent

namespace {
  void TriggerDictionaryInitialization_REXppRootEventDict_Impl() {
    static const char* headers[] = {
"include/REXppRootEvent.h",
0
    };
    static const char* includePaths[] = {
"./detectors/DetectionSetup/",
"./detectors/Strip/",
"./LISETools/",
"./SRIMTools/",
"./include/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/UNISim-tool/detectors/DetectionSetup/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/UNISim-tool/detectors/Strip/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/UNISim-tool/detectors/Fazia/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/UNISim-tool/detectors/Oscar/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/UNISim-tool/detectors/Silicon/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/UNISim-tool/detectors/Lamp/",
"/home/dellaquila/software/root-6.22.02/build/include/",
"/home/dellaquila/Dropbox/Ricerca/Ruder_Boskovic/software/REX++/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "REXppRootEventDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$include/REXppRootEvent.h")))  REXppRootEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "REXppRootEventDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "include/REXppRootEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"REXppRootEvent", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("REXppRootEventDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_REXppRootEventDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_REXppRootEventDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_REXppRootEventDict() {
  TriggerDictionaryInitialization_REXppRootEventDict_Impl();
}
