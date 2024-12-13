// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME DMSSoft_Dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
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

// Header files passed as explicit arguments
#include "DMSPlotter.h"
#include "DMSMathUtils.h"
#include "DMSDataProcess.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void delete_DMSPlotter(void *p);
   static void deleteArray_DMSPlotter(void *p);
   static void destruct_DMSPlotter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DMSPlotter*)
   {
      ::DMSPlotter *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::DMSPlotter >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("DMSPlotter", ::DMSPlotter::Class_Version(), "DMSPlotter.h", 18,
                  typeid(::DMSPlotter), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::DMSPlotter::Dictionary, isa_proxy, 4,
                  sizeof(::DMSPlotter) );
      instance.SetDelete(&delete_DMSPlotter);
      instance.SetDeleteArray(&deleteArray_DMSPlotter);
      instance.SetDestructor(&destruct_DMSPlotter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DMSPlotter*)
   {
      return GenerateInitInstanceLocal(static_cast<::DMSPlotter*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::DMSPlotter*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_DMSDataProcess(void *p = nullptr);
   static void *newArray_DMSDataProcess(Long_t size, void *p);
   static void delete_DMSDataProcess(void *p);
   static void deleteArray_DMSDataProcess(void *p);
   static void destruct_DMSDataProcess(void *p);
   static void streamer_DMSDataProcess(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DMSDataProcess*)
   {
      ::DMSDataProcess *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::DMSDataProcess >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("DMSDataProcess", ::DMSDataProcess::Class_Version(), "DMSDataProcess.h", 10,
                  typeid(::DMSDataProcess), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::DMSDataProcess::Dictionary, isa_proxy, 16,
                  sizeof(::DMSDataProcess) );
      instance.SetNew(&new_DMSDataProcess);
      instance.SetNewArray(&newArray_DMSDataProcess);
      instance.SetDelete(&delete_DMSDataProcess);
      instance.SetDeleteArray(&deleteArray_DMSDataProcess);
      instance.SetDestructor(&destruct_DMSDataProcess);
      instance.SetStreamerFunc(&streamer_DMSDataProcess);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DMSDataProcess*)
   {
      return GenerateInitInstanceLocal(static_cast<::DMSDataProcess*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::DMSDataProcess*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr DMSPlotter::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *DMSPlotter::Class_Name()
{
   return "DMSPlotter";
}

//______________________________________________________________________________
const char *DMSPlotter::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DMSPlotter*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int DMSPlotter::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DMSPlotter*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *DMSPlotter::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DMSPlotter*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *DMSPlotter::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DMSPlotter*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr DMSDataProcess::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *DMSDataProcess::Class_Name()
{
   return "DMSDataProcess";
}

//______________________________________________________________________________
const char *DMSDataProcess::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DMSDataProcess*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int DMSDataProcess::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::DMSDataProcess*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *DMSDataProcess::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DMSDataProcess*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *DMSDataProcess::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::DMSDataProcess*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void DMSPlotter::Streamer(TBuffer &R__b)
{
   // Stream an object of class DMSPlotter.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(DMSPlotter::Class(),this);
   } else {
      R__b.WriteClassBuffer(DMSPlotter::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_DMSPlotter(void *p) {
      delete (static_cast<::DMSPlotter*>(p));
   }
   static void deleteArray_DMSPlotter(void *p) {
      delete [] (static_cast<::DMSPlotter*>(p));
   }
   static void destruct_DMSPlotter(void *p) {
      typedef ::DMSPlotter current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::DMSPlotter

//______________________________________________________________________________
void DMSDataProcess::Streamer(TBuffer &R__b)
{
   // Stream an object of class DMSDataProcess.

   TObject::Streamer(R__b);
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_DMSDataProcess(void *p) {
      return  p ? new(p) ::DMSDataProcess : new ::DMSDataProcess;
   }
   static void *newArray_DMSDataProcess(Long_t nElements, void *p) {
      return p ? new(p) ::DMSDataProcess[nElements] : new ::DMSDataProcess[nElements];
   }
   // Wrapper around operator delete
   static void delete_DMSDataProcess(void *p) {
      delete (static_cast<::DMSDataProcess*>(p));
   }
   static void deleteArray_DMSDataProcess(void *p) {
      delete [] (static_cast<::DMSDataProcess*>(p));
   }
   static void destruct_DMSDataProcess(void *p) {
      typedef ::DMSDataProcess current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_DMSDataProcess(TBuffer &buf, void *obj) {
      ((::DMSDataProcess*)obj)->::DMSDataProcess::Streamer(buf);
   }
} // end of namespace ROOT for class ::DMSDataProcess

namespace {
  void TriggerDictionaryInitialization_DMSSoft_Dict_Impl() {
    static const char* headers[] = {
"DMSPlotter.h",
"DMSMathUtils.h",
"DMSDataProcess.h",
nullptr
    };
    static const char* includePaths[] = {
"/home/wyjang/.local/include/",
"/home/wyjang/Dropbox/Works/DAMSA/2024_BackgroundMeasurement/DAMSA_OfflineSoftware/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "DMSSoft_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$DMSPlotter.h")))  DMSPlotter;
class __attribute__((annotate("$clingAutoload$DMSDataProcess.h")))  DMSDataProcess;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "DMSSoft_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "DMSPlotter.h"
#include "DMSMathUtils.h"
#include "DMSDataProcess.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"DMSDataProcess", payloadCode, "@",
"DMSPlotter", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("DMSSoft_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_DMSSoft_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_DMSSoft_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_DMSSoft_Dict() {
  TriggerDictionaryInitialization_DMSSoft_Dict_Impl();
}
