// Copyright Sean Payne All Rights Reserved.

#include "PythonWrapper.h"
#include "IPythonScriptPlugin.h"
#include "FleetManagerModule.h"

// returns a string = the eval string has to be one line... I think
FString PythonWrapper::RunPythonEval(const FString& EvalString)
{
	FPythonCommandEx PythonCommand2;
	PythonCommand2.FileExecutionScope = EPythonFileExecutionScope::Public;
	PythonCommand2.Command = EvalString;
	PythonCommand2.ExecutionMode = EPythonCommandExecutionMode::EvaluateStatement;
	IPythonScriptPlugin::Get()->ExecPythonCommandEx(PythonCommand2);
	UE_LOG(FleetManagerModule, Warning, TEXT("got %s"), *PythonCommand2.CommandResult);
	return PythonCommand2.CommandResult;
}

// returns the result from the second call
FString PythonWrapper::RunPythonPair(const FString &SetupString, const FString &EvalString)
{
	FPythonCommandEx PythonCommand;
	PythonCommand.FileExecutionScope = EPythonFileExecutionScope::Public;
	PythonCommand.Command = SetupString;
	IPythonScriptPlugin::Get()->ExecPythonCommandEx(PythonCommand);

	FString result = RunPythonEval(EvalString);
	return result;
}