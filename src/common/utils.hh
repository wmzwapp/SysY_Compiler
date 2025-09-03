#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

enum class ObjType : uint32_t {
    /* AST begin */
	BaseAST,
    MulExpAST,
	AddExpAST,
	RelExpAST,
	EqExpAST,
	LAndExpAST,
	LOrExpAST,
    /* AST end */

    /* IR begin */
	IRBase,

	// value
	ValueIntIR,
	SymbolIR,

	// entity
	ProgramIR,
	FunctionIR,
	BlockIR,
	StmtRetIR,
	StmtBinaryExprIR,
    /* IR begin */
};

#define SET_TYPE_ID(T) set_type_id(ObjType::T)

template <typename T>
concept mytype = requires(T t) {
	{ t->get_type_id() } -> std::convertible_to<ObjType>;
};

template <typename T>
concept has_type_id = requires(T t) {
	{ T::TYPE_ID_ };
};

template<has_type_id CHECK_T, typename OBJ_T>
requires std::is_pointer_v<OBJ_T> && mytype<OBJ_T>
inline bool isa(OBJ_T obj) {
    if (obj == nullptr) {
        return false;
    }

	return obj->get_type_id() == CHECK_T::TYPE_ID_;
}
