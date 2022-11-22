// Copyright Sean Payne All Rights Reserved.
#pragma once
#include "CoreTypes.h"

namespace PythonWrapper {
	FString RunPythonEval(const FString& EvalString);
	FString RunPythonPair(const FString& SetupString, const FString& EvalString);
}
