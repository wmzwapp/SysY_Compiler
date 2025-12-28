#include "VarAST.hh"
#include "ir/TypeIR.hh"

TypeIR* TypeAST::get_ty_IR() {
    return mmpool_.make<TypeIntIR>();
}