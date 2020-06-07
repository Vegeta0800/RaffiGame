
#pragma once
// EXTERNAL INCLUDES
#include <comdef.h>
#include <cstdio>
#include <iostream>
#include <chrono>
//INTERNAL INCLUDES 


// Converts degrees to radians.
#define DegToRad(angleDegrees) (angleDegrees * M_PI / 180.0f)

// Converts radians to degrees.
#define RadToDeg(angleRadians) (angleRadians * 180.0f / M_PI)

//Defines for pointer. Delete, Delete an array and release.
#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x = nullptr; }
#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; }

//Define a forbid for the copy constructor.
#define FORBID_COPY(Type)					\
private:									\
	Type(const Type&);						

//Define a forbid for the constructor.
#define FORBID_CONSTRUCTION(Type)			\
private:									\
	Type() { };								\

//Functions to get the instance or instance pointer. 
//Defines function to delete the instance.
#define DEFINE_SINGLETON(Type)				\
FORBID_COPY(Type)							\
FORBID_CONSTRUCTION(Type)					\
public:										\
	static Type& GetInstance(void)			\
	{										\
		if (!instance)						\
		{									\
			instance = new Type();			\
		}									\
		return *instance;					\
	}										\
	static Type* GetInstancePtr(void)		\
	{										\
		if (!instance)						\
		{									\
			instance = new Type();			\
		}									\
		return instance;					\
	}										\
	static void Release(void)				\
	{										\
		SAFE_DELETE(instance)				\
	}										\
											\
protected:									\
	static Type* instance;

//Declare a Singleton
#define DECLARE_SINGLETON(Type)				\
Type* Type::instance = nullptr;

//Get variable name
#define GetVariableName(var) #var

//Set string to a char array
#define SET_STRING(x, y, size) { for (int j = 0; j < size; j++) { x[j] = y[j]; }}