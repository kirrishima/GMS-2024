#pragma once
#include "FST.h"

namespace REGEX
{
	FST::FST* CreateIntegerFST(const char* str);
	FST::FST* CreateCharFST(const char* str);
	FST::FST* CreateStringFST(const char* str);
	FST::FST* CreateMainFST(const char* str);
	FST::FST* CreatePrintFST(const char* str);
	FST::FST* CreateWriteFST(const char* str);
	FST::FST* CreateStrcmpFST(const char* str);
	FST::FST* CreateReturnFST(const char* str);
	FST::FST* CreateIntDECIMALLiteralFST(const char* str);
	FST::FST* CreateIntHEXLiteralFST(const char* str);
	FST::FST* CreateIntOCTALLiteralFST(const char* str);
	FST::FST* CreateIntBINARYLiteralFST(const char* str);
	FST::FST* CreateIdentifierFST(const char* str);
	FST::FST* CreateElseFST(const char* str);
	FST::FST* CreateIfFST(const char* str);
	FST::FST* CreateCompareFST(const char* str);
}