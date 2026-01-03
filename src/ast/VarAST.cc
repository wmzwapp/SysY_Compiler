#include "VarAST.hh"
#include "ir/TypeIR.hh"

IR::Type* TypeAST::get_ty_IR() {
    return mmpool_.make<IR::TypeInt>();
}