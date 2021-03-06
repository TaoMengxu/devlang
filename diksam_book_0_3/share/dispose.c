#include "MEM.h"
#include "DBG.h"
#include "DVM_code.h"

static void dispose_local_variable(int local_variable_count,
                                   DVM_LocalVariable *local_variable_list);

static void
dispose_type_derive(DVM_TypeSpecifier *type)
{
    int i;

    for (i = 0; i < type->derive_count; i++) {
        switch (type->derive[i].tag) {
        case DVM_FUNCTION_DERIVE:
            dispose_local_variable(type->derive[i].u
                                   .function_d.parameter_count,
                                   type->derive[i].u
                                   .function_d.parameter);
            break;
        case DVM_ARRAY_DERIVE:
            break;
        default:
            DBG_assert(0, ("derive->tag..%d\n", type->derive[i].tag));
        }
    }
    MEM_free(type->derive);
}

static void
dispose_type_specifier(DVM_TypeSpecifier *type)
{
    dispose_type_derive(type);
    MEM_free(type);
}

static void
dispose_local_variable(int local_variable_count,
                       DVM_LocalVariable *local_variable)
{
    int i;

    for (i = 0; i < local_variable_count; i++) {
        MEM_free(local_variable[i].name);
        dispose_type_specifier(local_variable[i].type);
    }
    MEM_free(local_variable);
}

static void
dispose_class(DVM_Class *cd)
{
    int i;

    for (i = 0; i < cd->field_count; i++) {
        MEM_free(cd->field[i].name);
        dispose_type_specifier(cd->field[i].type);
    }
    MEM_free(cd->field);

    for (i = 0; i < cd->method_count; i++) {
        MEM_free(cd->method[i].name);
    }
    MEM_free(cd->method);

    if (cd->super_class) {
        MEM_free(cd->super_class->package_name);
        MEM_free(cd->super_class->name);
        MEM_free(cd->super_class);
    }

    for (i = 0; i < cd->interface_count; i++) {
        MEM_free(cd->interface_[i].package_name);
        MEM_free(cd->interface_[i].name);
    }
    MEM_free(cd->interface_);

    MEM_free(cd->package_name);
    MEM_free(cd->name);
}

void
dvm_dispose_executable(DVM_Executable *exe)
{
    int i;

    MEM_free(exe->package_name);
    MEM_free(exe->path);

    for (i = 0; i < exe->constant_pool_count; i++) {
        if (exe->constant_pool[i].tag == DVM_CONSTANT_STRING) {
            MEM_free(exe->constant_pool[i].u.c_string);
        }
    }
    MEM_free(exe->constant_pool);
    
    for (i = 0; i < exe->global_variable_count; i++) {
        MEM_free(exe->global_variable[i].name);
        dispose_type_specifier(exe->global_variable[i].type);
    }
    MEM_free(exe->global_variable);

    for (i = 0; i < exe->function_count; i++) {
        MEM_free(exe->function[i].name);
        MEM_free(exe->function[i].package_name);
        dispose_type_specifier(exe->function[i].type);
        dispose_local_variable(exe->function[i].parameter_count,
                               exe->function[i].parameter);
        if (exe->function[i].is_implemented) {
            dispose_local_variable(exe->function[i].local_variable_count,
                                   exe->function[i].local_variable);
            MEM_free(exe->function[i].code);
            MEM_free(exe->function[i].line_number);
        }
    }
    MEM_free(exe->function);

    for (i = 0; i < exe->type_specifier_count; i++) {
        dispose_type_derive(&exe->type_specifier[i]);
    }
    MEM_free(exe->type_specifier);

    for (i = 0; i < exe->class_count; i++) {
        dispose_class(&exe->class_definition[i]);
    }
    MEM_free(exe->class_definition);

    MEM_free(exe->code);
    MEM_free(exe->line_number);
    MEM_free(exe);
}
