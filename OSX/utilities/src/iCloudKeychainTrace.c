/*
 * Copyright (c) 2013-2014 Apple Inc. All Rights Reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */


#include "iCloudKeychainTrace.h"
#include <TargetConditionals.h>
#include <inttypes.h>
#include "SecCFWrappers.h"
#include <sys/time.h>
#include <CoreFoundation/CoreFoundation.h>

const CFStringRef kNumberOfiCloudKeychainPeers = CFSTR("numberOfPeers");
const CFStringRef kNumberOfiCloudKeychainItemsBeingSynced = CFSTR("numberOfItemsBeingSynced");
const CFStringRef kCloudKeychainNumberOfSyncingConflicts = CFSTR("conflictsCount");
const CFStringRef kCloudKeychainNumberOfTimesSyncFailed = CFSTR("syncFailureCount");
const CFStringRef kCloudKeychainNumberOfConflictsResolved = CFSTR("conflictsResolved");
const CFStringRef kCloudKeychainNumberOfTimesSyncedWithPeers = CFSTR("syncedWithPeers");

#if !TARGET_IPHONE_SIMULATOR
#if TARGET_OS_IPHONE
static const char* gTopLevelKeyForiCloudKeychainTracing = "com.apple.icdp";
#else
static const char* gTopLevelKeyForiCloudKeychainTracing = "com.apple.icdp.KeychainStats";
#endif
#endif

#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR))
#include <msgtracer_client.h>

struct msgtracer_instance {
    msgtracer_msg_t message;
    msgtracer_domain_t domain;
};

static const char* gMessageTracerSetPrefix = "com.apple.message.";

/* --------------------------------------------------------------------------
	Function:		OSX_BeginCloudKeychainLoggingTransaction
	
	Description:	For OSX the message tracer back end wants its logging 
					done in "bunches".  This function allows for beginning
					a 'transaction' of logging which will allow for putting
					all of the transactions items into a single log making
					the message tracer folks happy.
					
                    The work of this function is to ask msgtracer for a domain,
                    create a message and return the pair as a void *.
   -------------------------------------------------------------------------- */					
static void *OSX_BeginCloudKeychainLoggingTransaction()
{
    
    struct msgtracer_instance *instance = calloc(1, sizeof *instance);
    if (!instance) {
        return NULL;
    }

    instance->domain = msgtracer_domain_new(gTopLevelKeyForiCloudKeychainTracing);
    if (instance->domain) {
        instance->message = msgtracer_msg_new(instance->domain);
        if (instance->message) {
            return (void *)instance;
        }
        msgtracer_domain_free(instance->domain);
    }
    free(instance);
    return NULL;
}

/* --------------------------------------------------------------------------
	Function:		OSX_AddKeyValuePairToKeychainLoggingTransaction
	
	Description:	Once a call to OSX_BeginCloudKeychainLoggingTransaction 
					is done, this call all allow for adding items to the
					"bunch" of items being logged.
					
					NOTE: The key should be a simple key such as 
					"numberOfPeers".  This is because this function will 
					prepend the required prefix of "com.apple.message."
   -------------------------------------------------------------------------- */
static bool OSX_AddKeyValuePairToKeychainLoggingTransaction(void *token, CFStringRef key, int64_t value)
{
	if (NULL == token || NULL == key)
	{
		return false;
	}

    struct msgtracer_instance *instance = (struct msgtracer_instance *)token;
	msgtracer_msg_t msg = instance->message;
	
	// Fix up the key
	__block char *real_key = NULL;
	CFStringPerformWithCString(key, ^(const char *key_utf8) {
		asprintf(&real_key, "%s%s", gMessageTracerSetPrefix, key_utf8);
	});
	if (NULL == real_key)
	{
		return false;
	}
	
	char value_buffer[32];
	snprintf(value_buffer, sizeof(value_buffer), "%lld", value);

	msgtracer_set(msg, real_key, value_buffer);
	free(real_key);
	return true;	
}

/* --------------------------------------------------------------------------
	Function:		OSX_CloseCloudKeychainLoggingTransaction
	
	Description:	Once a call to OSX_BeginCloudKeychainLoggingTransaction 
					is done, and all of the items that are to be in the 
					"bunch" of items being logged, this function will do the
					real logging and free the aslmsg context.
   -------------------------------------------------------------------------- */
static void OSX_CloseCloudKeychainLoggingTransaction(void *token)
{
	if (NULL != token)
	{
        struct msgtracer_instance *instance = (struct msgtracer_instance *)token;
        msgtracer_log(instance->message, ASL_LEVEL_NOTICE, "");
        msgtracer_msg_free(instance->message);
        msgtracer_domain_free(instance->domain);
        free(instance);
	}
}

/* --------------------------------------------------------------------------
	Function:		OSX_SetCloudKeychainTraceValueForKey
	
	Description:	If "bunching" of items either cannot be done or is not
					desired, then this 'single shot' function shold be used.
					It will create the aslmsg context, register the domain
					fix up the key and log the key value pair and then 
					do the real logging and free the aslmsg context.
   -------------------------------------------------------------------------- */
static bool OSX_SetCloudKeychainTraceValueForKey(CFStringRef key, int64_t value)
{
	bool result = false;
	
	if (NULL == key)
	{
		return result;
	}

    msgtracer_msg_t message = NULL;
    msgtracer_domain_t domain = msgtracer_domain_new(gTopLevelKeyForiCloudKeychainTracing);

    if (NULL == domain) {
        return result;
    }

    message = msgtracer_msg_new(domain);
    if (NULL == message) {
        msgtracer_domain_free(domain);
        return result;
    }

    // Fix up the key
	__block char *real_key = NULL;
	CFStringPerformWithCString(key, ^(const char *key_utf8) {
		asprintf(&real_key, "%s%s", gMessageTracerSetPrefix, key_utf8);
	});
	if (NULL == real_key)
	{
		return false;
	}
	
	char value_buffer[32];
	snprintf(value_buffer, sizeof(value_buffer), "%lld", value);

    msgtracer_set(message, real_key, value_buffer);
	msgtracer_log(message, ASL_LEVEL_NOTICE, "%s is %lld", real_key, value);
    msgtracer_msg_free(message);
    msgtracer_domain_free(domain);
    free(real_key);
	return true;
	
}
#endif

#if (TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR)

typedef void (*type_ADClientClearScalarKey)(CFStringRef key);
typedef void (*type_ADClientSetValueForScalarKey)(CFStringRef key, int64_t value);

static type_ADClientClearScalarKey gADClientClearScalarKey = NULL;
static type_ADClientSetValueForScalarKey gADClientSetValueForScalarKey = NULL;

static dispatch_once_t gADFunctionPointersSet = 0;
static CFBundleRef gAggdBundleRef = NULL;
static bool gFunctionPointersAreLoaded = false;

/* --------------------------------------------------------------------------
	Function:		InitializeADFunctionPointers
	
	Description:	Linking to the Aggregate library causes a build cycle so
					This function will dynamically load the needed function
					pointers.
   -------------------------------------------------------------------------- */
static bool InitializeADFunctionPointers()
{    
	if (gFunctionPointersAreLoaded)
	{
		return gFunctionPointersAreLoaded;
	}
    
    dispatch_once(&gADFunctionPointersSet,
      ^{
          CFStringRef path_to_aggd_framework = CFSTR("/System/Library/PrivateFrameworks/AggregateDictionary.framework");
          
          CFURLRef aggd_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, path_to_aggd_framework, kCFURLPOSIXPathStyle, true);
          
          if (NULL != aggd_url)
          {
              gAggdBundleRef = CFBundleCreate(kCFAllocatorDefault, aggd_url);
              if (NULL != gAggdBundleRef)
              {  
                  gADClientClearScalarKey = (type_ADClientClearScalarKey)
                    CFBundleGetFunctionPointerForName(gAggdBundleRef, CFSTR("ADClientClearScalarKey"));
                  
                  gADClientSetValueForScalarKey = (type_ADClientSetValueForScalarKey)
                    CFBundleGetFunctionPointerForName(gAggdBundleRef, CFSTR("ADClientSetValueForScalarKey"));
              }
              CFRelease(aggd_url);
          }
      });
    
    gFunctionPointersAreLoaded = ((NULL != gADClientClearScalarKey) && (NULL != gADClientSetValueForScalarKey));
    return gFunctionPointersAreLoaded;
}

/* --------------------------------------------------------------------------
	Function:		Internal_ADClientClearScalarKey
	
	Description:	This fucntion is a wrapper around calling the  
					ADClientClearScalarKey function.  
					
					NOTE: The key should be a simple key such as 
					"numberOfPeers".  This is because this function will 
					apptend the required prefix of "com.apple.cloudkeychain"
   -------------------------------------------------------------------------- */
static void Internal_ADClientClearScalarKey(CFStringRef key)
{
    if (InitializeADFunctionPointers())
    {
		CFStringRef real_key = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%s.%@"), gTopLevelKeyForiCloudKeychainTracing, key);
		if (NULL == real_key)
		{
			return;
		}
		
        gADClientClearScalarKey(real_key);
		CFRelease(real_key);
    }
}

/* --------------------------------------------------------------------------
	Function:		Internal_ADClientSetValueForScalarKey
	
	Description:	This fucntion is a wrapper around calling the  
					ADClientSetValueForScalarKey function.
					
					NOTE: The key should be a simple key such as 
					"numberOfPeers".  This is because this function will 
					apptend the required prefix of "com.apple.cloudkeychain"
   -------------------------------------------------------------------------- */
static void Internal_ADClientSetValueForScalarKey(CFStringRef key, int64_t value)
{
    if (InitializeADFunctionPointers())
    {
		CFStringRef real_key = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%s.%@"), gTopLevelKeyForiCloudKeychainTracing, key);
		if (NULL == real_key)
		{
			return;
		}
		
        gADClientSetValueForScalarKey(real_key, value);
		CFRelease(real_key);
    }
}


/* --------------------------------------------------------------------------
	Function:		iOS_SetCloudKeychainTraceValueForKey
	
	Description:	This fucntion is a wrapper around calling either   
					ADClientSetValueForScalarKey  or ADClientClearScalarKey
					depending on if the value is 0.
					
					NOTE: The key should be a simple key such as 
					"numberOfPeers".  This is because this function will 
					apptend the required prefix of "com.apple.cloudkeychain"
   -------------------------------------------------------------------------- */
static bool iOS_SetCloudKeychainTraceValueForKey(CFStringRef key, int64_t value)
{
	if (NULL == key)
	{
		return false;
	}
    
    if (0LL == value)
    {
        Internal_ADClientClearScalarKey(key);
    }
    else
    {
        Internal_ADClientSetValueForScalarKey(key, value);
    }
	return true;
}

/* --------------------------------------------------------------------------
	Function:		iOS_AddKeyValuePairToKeychainLoggingTransaction
	
	Description:	For iOS the is no "bunching"  This function will simply
					call iOS_SetCloudKeychainTraceValueForKey to log the 
					key value pair
   -------------------------------------------------------------------------- */
static bool iOS_AddKeyValuePairToKeychainLoggingTransaction(void* token, CFStringRef key, int64_t value)
{
#pragma unused(token)
	return iOS_SetCloudKeychainTraceValueForKey(key, value);
}
#endif

/* --------------------------------------------------------------------------
	Function:		SetCloudKeychainTraceValueForKey
	
	Description:	SPI to log a single key value pair with the logging system
   -------------------------------------------------------------------------- */
bool SetCloudKeychainTraceValueForKey(CFStringRef key, int64_t value)
{
#if (TARGET_IPHONE_SIMULATOR)
	return false;
#endif 

#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE))
	return OSX_SetCloudKeychainTraceValueForKey(key, value);
#endif

#if (TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR)
	return iOS_SetCloudKeychainTraceValueForKey(key, value);
#endif
}

/* --------------------------------------------------------------------------
	Function:		BeginCloudKeychainLoggingTransaction
	
	Description:	SPI to begin a logging transaction
   -------------------------------------------------------------------------- */
void* BeginCloudKeychainLoggingTransaction()
{
#if (TARGET_IPHONE_SIMULATOR)
	return (void *)-1;
#endif

#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE))
	return OSX_BeginCloudKeychainLoggingTransaction();
#endif

#if (TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR)
	return NULL;
#endif
}

/* --------------------------------------------------------------------------
	Function:		AddKeyValuePairToKeychainLoggingTransaction
	
	Description:	SPI to add a key value pair to an outstanding logging
					tansaction
   -------------------------------------------------------------------------- */
bool AddKeyValuePairToKeychainLoggingTransaction(void* token, CFStringRef key, int64_t value)
{
#if (TARGET_IPHONE_SIMULATOR)
	return false;
#endif

#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE))
	return OSX_AddKeyValuePairToKeychainLoggingTransaction(token, key, value);
#endif

#if (TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR)
	return iOS_AddKeyValuePairToKeychainLoggingTransaction(token, key, value);
#endif
}

/* --------------------------------------------------------------------------
	Function:		CloseCloudKeychainLoggingTransaction
	
	Description:	SPI to complete a logging transaction and clean up the
					context
   -------------------------------------------------------------------------- */
void CloseCloudKeychainLoggingTransaction(void* token)
{
#if (TARGET_IPHONE_SIMULATOR)
	; // nothing
#endif
	
#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE))
	OSX_CloseCloudKeychainLoggingTransaction(token);
#endif

#if (TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR)
	; // nothing
#endif
}

