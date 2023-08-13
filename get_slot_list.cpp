/*
        This program demonstrates how to use C_GetSlotList to get the list of all detected slots.
*/


#include <iostream>
#include "../include/pkcs11.h"
#include "../include/cryptoki.h"
#include <stdlib.h>
#include <string.h>
#pragma warning(disable: 4996)

using namespace std;

// OS Check
#ifdef NIX
        #include <dlfcn.h>
#else
        #include <windows.h>
#endif

// OS Check
#ifdef NIX
        void *libHandle = 0;
#else
        HINSTANCE libHandle = 0;
#endif

CK_FUNCTION_LIST *p11Func = NULL;
CK_SLOT_ID *slots = NULL;
const char *libPath;

typedef struct{
     int x;
}MySecretStruct;
// This function loads a pkcs11 library. Path of the pkcs11 library is read using P11_LIB environment variable.
void loadHSMLibrary()
{
	libPath = getenv("P11_LIB");
	if(libPath==NULL)
	{
		cout << "P11_LIB environment variable not set." << endl;
		exit(1);
	}

	// OS Check
        #ifdef NIX
                libHandle = dlopen(libPath, RTLD_NOW);
        #else
                libHandle = LoadLibraryA(libPath);
        #endif

	if(!libHandle)
	{
		cout << "Failed to load pkcs#11 library. " << libPath << endl;
		exit(1);
	}

	// OS Check
        #ifdef NIX
                CK_C_GetFunctionList C_GetFunctionList = (CK_C_GetFunctionList)dlsym(libHandle,"C_GetFunctionList");
        #else
                CK_C_GetFunctionList C_GetFunctionList = (CK_C_GetFunctionList)GetProcAddress(libHandle,"C_GetFunctionList");
        #endif

	C_GetFunctionList(&p11Func);
	if(!p11Func)
	{
		cout << "Failed to load pkcs#11 functions." << endl;
		exit(1);
	}
}




// Before exiting, this functions performs some memory cleanup.
void freeResource()
{
	// OS Check
	#ifdef NIX
        	dlclose(libHandle);
	#else
		FreeLibrary(libHandle);
	#endif
        p11Func = NULL;
	slots = NULL;
}




// This function checks if a requested PKCS #11 operation was a success or a failure. 
void checkOperation(CK_RV rv, const char *message)
{
	if(rv!=CKR_OK)
	{
		cout << message << " failed with : " << rv << endl;
		freeResource();
		exit(1);
	}
}



// This function gets the list of all detected slots..
void show_all_slots()
{
	CK_ULONG no_of_slots;

	checkOperation(p11Func->C_GetSlotList(CK_TRUE, NULL_PTR, &no_of_slots), "C_GetSlotList");
	cout << no_of_slots << " slots detected." << endl;
	slots = new CK_SLOT_ID[no_of_slots];
	checkOperation(p11Func->C_GetSlotList(CK_TRUE, slots, &no_of_slots), "C_GetSlotList");
	
	for(int ctr=0; ctr<no_of_slots; ctr++)
	{
		cout << slots[ctr] << endl;
	}
	
}



int main(int argc, char **argv)
{
	int ctr=0;
	loadHSMLibrary();
	cout << "PKCS#11 library loaded." << endl;
	
	// Initialize Cryptoki, call C_GetSlotList and then C_Finalize
	checkOperation(p11Func->C_Initialize(NULL_PTR), "C_Initialize");
        show_all_slots();
	checkOperation(p11Func->C_Finalize(NULL_PTR), "C_Finalize");

	freeResource();
	return 0;
}



/*
                                *** DISCLAIMER ***
        - THIS CODE IS INTENDED TO BE USED FOR DEMO or TESTING PURPOSE ONLY.
        - IT DOES NOT HAVE THE PERFECT INGREDIENTS FOR YOUR COPY PASTA.
        - READ > UNDERSTAND > REVIEW > REPEAT...TILL YOU KNOW WHAT YOU'RE DOING.
        - WHATEVER IT IS... NOT MY PROBLEM, NOT LIABLE, NOT RESPONSIBLE !!
*/
