#pragma once
#include <variant>
#include <vector>
#include "access_flags.h"
#include "annotation.h"
#include "detail/types.h"
#include "stack_map.h"

namespace classcafe
{
    namespace Attribute
    {
        struct ConstantValue { u2 constantvalue_index; };

        struct Info;

        struct Code {
            struct Exception {
                u2 start_pc;
                u2 end_pc;
                u2 handler_pc;
                u2 catch_type;
            };

            u2 max_stack;
            u2 max_locals;
            Vec<u1> code;
            Vec<Exception> exception_table;
            Vec<Attribute::Info> attributes;
        };

        struct StackMapTable {
            Vec<StackMap::Frame> entries;
        };

        struct Exceptions {
            Vec<u2> exception_index_table;
        };

        struct InnerClasses {
            struct Data {
                u2 inner_class_info_index;
                u2 outer_class_info_index;
                u2 inner_name_index;
                InnerClassAccess inner_class_access_flags;
            };

            Vec<Data> classes;
        };

        struct EnclosingMethod {
            u2 class_index;
            u2 method_index;
        };

        struct Synthetic {};

        struct Signature { u2 signature_index; };

        struct SourceFile { u2 sourcefile_index; };

        struct SourceDebugExtension { Vec<u1> debug_extension; };

        struct LineNumberTable {
            struct LineNumber {
                u2 start_pc;
                u2 line_number;
            };

            Vec<LineNumber> line_number_table;
        };

        struct LocalVariableTable {
            struct Local {
                u2 start_pc;
                u2 length;
                u2 name_index;
                u2 descriptor_index;
                u2 index;
            };

            Vec<Local> local_variable_table;
        };

        struct LocalVariableTypeTable {
            struct Local {
                u2 start_pc;
                u2 length;
                u2 name_index;
                u2 signature_index;
                u2 index;
            };
            
            Vec<Local> local_variable_type_table;
        };

        struct Deprecated {};

        struct RuntimeVisibleAnnotations {
            Vec<Annotations::Annotation> annotations;
        };

        struct RuntimeInvisibleAnnotations {
            Vec<Annotations::Annotation> annotations;
        };

        struct RuntimeVisibleParameterAnnotations {
            Vec<Vec<Annotations::Annotation>> parameter_annotations;
        };

        struct RuntimeInvisibleParameterAnnotations {
            Vec<Vec<Annotations::Annotation>> parameter_annotations;
        };

        struct AnnotationDefault {
            Annotations::ElementValue element_value;
        };

        struct BootstrapMethods {
            struct Method {
                u2 bootstrap_method_ref;
                Vec<u2> bootstrap_arguments;
            };

            Vec<Method> bootstrap_methods;
        };

        struct Info {
            using Kind = std::variant<ConstantValue, Code, StackMapTable, Exceptions, InnerClasses, EnclosingMethod, 
                Synthetic, Signature, SourceFile, SourceDebugExtension,
                LineNumberTable, LocalVariableTable, LocalVariableTypeTable, 
                Deprecated, 
                RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations,
                RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations,
                AnnotationDefault, BootstrapMethods>;
            u2 attribute_name_index;
            Kind kind;
        };
    } // Attribute
}