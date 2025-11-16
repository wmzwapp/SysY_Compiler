#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

enum class ObjType : uint32_t {
    /* AST begin */
	BaseAST,
	BlockAST,
	BlockItemAST,
	DeclAST,
	StmtAST,
	ConstDeclAST,
	BTypeAST,
	ConstDefAST,
	ConstInitValAST,
	VarDeclAST,
	VarDefAST,
	InitValAST,

	ExpAST,
	PrimaryExpAST,
	UnaryExpAST,
    MulExpAST,
	AddExpAST,
	RelExpAST,
	EqExpAST,
	LAndExpAST,
	LOrExpAST,
	ConstExpAST,
	ExpASTEnd,

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
	AllocIR,
	StoreIR,
	LoadIR,
    /* IR end */
};

#define SET_TYPE_ID(T) set_type_id(ObjType::T)

#define STATIC_TYPE_ID_DECL(T) inline static ObjType TYPE_ID_ { ObjType::T }

template <typename T>
concept has_type_id = requires {
	{ std::remove_pointer_t<T>::TYPE_ID_ };
};

template<typename T>
concept mytype_nonptr = requires(T t) {
	{ t.get_type_id() } -> std::convertible_to<ObjType>;
};

template<typename T>
concept mytype_ptr = requires(T t) {
	{ t->get_type_id() } -> std::convertible_to<ObjType>;
};

template <typename T>
concept mytype = mytype_nonptr<T> || mytype_ptr<T>;

template<has_type_id CHECK_T, mytype OBJ_T>
requires (std::is_pointer_v<CHECK_T> == std::is_pointer_v<OBJ_T>)
inline bool isa(OBJ_T obj) {
    if (obj == nullptr) {
        return false;
    }

	if constexpr (std::is_pointer_v<CHECK_T>) {
		return obj->get_type_id() == std::remove_pointer_t<CHECK_T>::TYPE_ID_;
	} else {
		return obj.get_type_id() == CHECK_T::TYPE_ID_;
	}
}

template<mytype T>
inline bool is_exp_family(T&& obj) {
	using U = std::remove_reference_t<T>;
	if constexpr (std::is_pointer_v<U>) {
		if (!obj) return false;
		const auto id = obj->get_type_id();
		return id > ObjType::ExpAST && id < ObjType::ExpASTEnd;
	} else {
		const auto id = obj.get_type_id();
		return id > ObjType::ExpAST && id < ObjType::ExpASTEnd;
	}
}
