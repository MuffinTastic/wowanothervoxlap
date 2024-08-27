#include "scripting.h"

#include <cstdio>
#include <string>
#include <Windows.h>

#include "AngelScript\include\angelscript.h"
#include "AngelScript\addons\scriptstdstring.h"
#include "AngelScript\addons\scriptarray.h"
#include "AngelScript\addons\scriptbuilder.h"
#include "AngelScript\addons\scriptmath.h"

#include "scriptbridge.h"

void script_message_callback(const asSMessageInfo *msg, void *param);
void showError(const char *format, ...);
void script_showerror();

asIScriptEngine *script_engine;
asIScriptContext *script_context;

asIScriptFunction *script_func_initapp;
asIScriptFunction *script_func_doframe;
asIScriptFunction *script_func_uninitapp;

std::string script_log;

void printstr(const std::string &message)
{
	printf("%s\n", message.c_str());
}

int script_engine_setup()
{
	int ret = script_engine->SetMessageCallback(asFUNCTION(script_message_callback), 0, asCALL_CDECL);
	if (ret < 0) {
		showError("Couldn't initialize scripting system:\n%s",
			"Could not set message callback");
		script_showerror();
		return 0;
	}

	RegisterStdString(script_engine);
	RegisterScriptArray(script_engine, true);
	RegisterScriptMath(script_engine);

	char *err_funcname = nullptr;
	ret = script_setup_bridge(script_engine, err_funcname);
	if (!ret) {
		showError("Could not initialize scripting system:\nCould not register declaration:\n%s", err_funcname);
		return 0;
	}

	CScriptBuilder builder;
	ret = builder.StartNewModule(script_engine, "voxtest");
	if (ret < 0) {
		script_showerror();
		return 0;
	}
	ret = builder.AddSectionFromFile("main.as");
	if (ret < 0) {
		script_showerror();
		return 0;
	}
	ret = builder.BuildModule();
	if (ret < 0) {
		script_showerror();
		return 0;
	}

	asIScriptModule *mod = script_engine->GetModule("voxtest");
	
	script_func_initapp = mod->GetFunctionByDecl("bool initapp()");
	if (script_func_initapp == 0) {
		showError("Could not find function:\n%s", "bool initapp()");
		return 0;
	}

	script_func_doframe = mod->GetFunctionByDecl("void doframe(double)");
	if (script_func_doframe == 0) {
		showError("Could not find function:\n%s", "void doframe(double)");
		return 0;
	}

	script_func_uninitapp = mod->GetFunctionByDecl("void uninitapp()");
	if (script_func_uninitapp == 0) {
		showError("Could not find function:\n%s", "void uninitapp()");
		return 0;
	}

	script_context->Prepare(script_func_initapp);
	ret = script_context->Execute();

	if (ret != asEXECUTION_FINISHED) {
		if (ret == asEXECUTION_EXCEPTION) {
			showError("Exception on line %d:\n%s", script_context->GetExceptionLineNumber(), script_context->GetExceptionString());
		}
	} else {
		asBYTE init_ret = script_context->GetReturnByte();
		if (init_ret) {
			script_context->Unprepare();
			showError("bool initapp() returned true -- quitting..");
			return 0;
		}
	}
	script_context->Unprepare();

	return 1;
}

int script_init()
{
	script_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	script_context = script_engine->CreateContext();
	if (!script_engine_setup()) {
		script_context->Release();
		script_engine->Release();
		return 0;
	}

	return 1;
}

void script_loop(double fsynctics)
{
	script_context->Prepare(script_func_doframe);
	script_context->SetArgDouble(0, fsynctics);
	int ret = script_context->Execute();
	if (ret != asEXECUTION_FINISHED) {
		if (ret == asEXECUTION_EXCEPTION) {
			showError("Exception on line %d:\n%s", script_context->GetExceptionLineNumber(), script_context->GetExceptionString());
		}
	}
	script_context->Unprepare();
}

void script_uninit()
{
	script_context->Prepare(script_func_uninitapp);
	int ret = script_context->Execute();
	if (ret != asEXECUTION_FINISHED) {
		if (ret == asEXECUTION_EXCEPTION) {
			showError("Exception on line %d:\n%s", script_context->GetExceptionLineNumber(), script_context->GetExceptionString());
		}
	}
	script_context->Unprepare();

	script_context->Release();
	script_engine->Release();
}

void script_message_callback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";
	char text[256];
	sprintf_s(text, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
	script_log.append(text);
}

void script_showerror()
{
	if (script_log.length() == 0)
		return;

	showError("%s", script_log.c_str());
	script_log.clear();
}

void showError(const char *format, ...)
{
	char* argslist;
	char text[4096];

	_crt_va_start(argslist, format);
	vsprintf_s(text, format, argslist);
	_crt_va_end(argslist);

	MessageBoxA(NULL, text, "Script Error", MB_OK);
}