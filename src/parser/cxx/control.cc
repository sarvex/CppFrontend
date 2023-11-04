// Copyright (c) 2023 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cxx/control.h>
#include <cxx/literals.h>
#include <cxx/memory_layout.h>
#include <cxx/names.h>
#include <cxx/symbols.h>
#include <cxx/types.h>

#include <cassert>
#include <cstdlib>
#include <forward_list>
#include <set>
#include <unordered_set>

namespace cxx {

namespace {

template <typename Literal>
struct LiteralHash {
  auto operator()(const Literal& literal) const -> std::size_t {
    return std::hash<std::string_view>{}(literal.value());
  }
};

template <typename Literal>
struct LiteralEqualTo {
  auto operator()(const Literal& lhs, const Literal& rhs) const -> bool {
    return lhs.value() == rhs.value();
  }
};

template <typename Literal>
using LiteralSet =
    std::unordered_set<Literal, LiteralHash<Literal>, LiteralEqualTo<Literal>>;

template <typename Name>
using NameSet = std::set<Name>;

}  // namespace

struct Control::Private {
  MemoryLayout* memoryLayout = nullptr;
  LiteralSet<IntegerLiteral> integerLiterals;
  LiteralSet<FloatLiteral> floatLiterals;
  LiteralSet<StringLiteral> stringLiterals;
  LiteralSet<CharLiteral> charLiterals;
  LiteralSet<WideStringLiteral> wideStringLiterals;
  LiteralSet<Utf8StringLiteral> utf8StringLiterals;
  LiteralSet<Utf16StringLiteral> utf16StringLiterals;
  LiteralSet<Utf32StringLiteral> utf32StringLiterals;
  LiteralSet<CommentLiteral> commentLiterals;

  std::set<Identifier> identifiers;
  std::set<OperatorId> operatorIds;
  std::set<DestructorId> destructorIds;
  std::set<LiteralOperatorId> literalOperatorIds;
  std::set<ConversionFunctionId> conversionFunctionIds;
  std::set<TemplateId> templateIds;

  VoidType voidType;
  NullptrType nullptrType;
  DecltypeAutoType decltypeAutoType;
  AutoType autoType;
  BoolType boolType;
  SignedCharType signedCharType;
  ShortIntType shortIntType;
  IntType intType;
  LongIntType longIntType;
  LongLongIntType longLongIntType;
  UnsignedCharType unsignedCharType;
  UnsignedShortIntType unsignedShortIntType;
  UnsignedIntType unsignedIntType;
  UnsignedLongIntType unsignedLongIntType;
  UnsignedLongLongIntType unsignedLongLongIntType;
  CharType charType;
  Char8Type char8Type;
  Char16Type char16Type;
  Char32Type char32Type;
  WideCharType wideCharType;
  FloatType floatType;
  DoubleType doubleType;
  LongDoubleType longDoubleType;
  ClassDescriptionType classDescriptionType;

  std::set<QualType> qualTypes;
  std::set<BoundedArrayType> boundedArrayTypes;
  std::set<UnboundedArrayType> unboundedArrayTypes;
  std::set<PointerType> pointerTypes;
  std::set<LvalueReferenceType> lvalueReferenceTypes;
  std::set<RvalueReferenceType> rvalueReferenceTypes;
  std::set<FunctionType> functionTypes;
  std::set<MemberObjectPointerType> memberObjectPointerTypes;
  std::set<MemberFunctionPointerType> memberFunctionPointerTypes;
  std::set<UnresolvedNameType> unresolvedNameTypes;
  std::set<UnresolvedBoundedArrayType> unresolvedBoundedArrayTypes;
  std::set<UnresolvedUnderlyingType> unresolvedUnderlyingTypes;

  std::forward_list<ClassType> classTypes;
  std::forward_list<UnionType> unionTypes;
  std::forward_list<NamespaceType> namespaceTypes;
  std::forward_list<EnumType> enumTypes;
  std::forward_list<ScopedEnumType> scopedEnumTypes;

  std::forward_list<NamespaceSymbol> namespaceSymbols;
  std::forward_list<ConceptSymbol> conceptSymbols;
  std::forward_list<ClassSymbol> classSymbols;
  std::forward_list<UnionSymbol> unionSymbols;
  std::forward_list<EnumSymbol> enumSymbols;
  std::forward_list<ScopedEnumSymbol> scopedEnumSymbols;
  std::forward_list<FunctionSymbol> functionSymbols;
  std::forward_list<LambdaSymbol> lambdaSymbols;
  std::forward_list<FunctionParametersSymbol> functionParametersSymbol;
  std::forward_list<TemplateParametersSymbol> templateParametersSymbol;
  std::forward_list<BlockSymbol> blockSymbols;
  std::forward_list<TypeAliasSymbol> typeAliasSymbols;
  std::forward_list<VariableSymbol> variableSymbols;
  std::forward_list<FieldSymbol> fieldSymbols;
  std::forward_list<ParameterSymbol> parameterSymbols;
  std::forward_list<TypeParameterSymbol> typeParameterSymbols;
  std::forward_list<NonTypeParameterSymbol> nonTypeParameterSymbols;
  std::forward_list<TemplateTypeParameterSymbol> templateTypeParameterSymbols;
  std::forward_list<ConstraintTypeParameterSymbol>
      constraintTypeParameterSymbols;
  std::forward_list<EnumeratorSymbol> enumeratorSymbols;

  int anonymousIdCount = 0;
};

Control::Control() : d(std::make_unique<Private>()) {}

Control::~Control() = default;

auto Control::integerLiteral(std::string_view spelling)
    -> const IntegerLiteral* {
  auto [it, inserted] = d->integerLiterals.emplace(std::string(spelling));
  if (inserted) it->initialize();
  return &*it;
}

auto Control::floatLiteral(std::string_view spelling) -> const FloatLiteral* {
  auto [it, inserted] = d->floatLiterals.emplace(std::string(spelling));
  if (inserted) it->initialize();
  return &*it;
}

auto Control::stringLiteral(std::string_view spelling) -> const StringLiteral* {
  auto [it, inserted] = d->stringLiterals.emplace(std::string(spelling));
  if (inserted) it->initialize();
  return &*it;
}

auto Control::charLiteral(std::string_view spelling) -> const CharLiteral* {
  auto [it, inserted] = d->charLiterals.emplace(std::string(spelling));
  if (inserted) it->initialize();
  return &*it;
}

auto Control::wideStringLiteral(std::string_view spelling)
    -> const WideStringLiteral* {
  return &*d->wideStringLiterals.emplace(std::string(spelling)).first;
}

auto Control::utf8StringLiteral(std::string_view spelling)
    -> const Utf8StringLiteral* {
  return &*d->utf8StringLiterals.emplace(std::string(spelling)).first;
}

auto Control::utf16StringLiteral(std::string_view spelling)
    -> const Utf16StringLiteral* {
  return &*d->utf16StringLiterals.emplace(std::string(spelling)).first;
}

auto Control::utf32StringLiteral(std::string_view spelling)
    -> const Utf32StringLiteral* {
  return &*d->utf32StringLiterals.emplace(std::string(spelling)).first;
}

auto Control::commentLiteral(std::string_view spelling)
    -> const CommentLiteral* {
  return &*d->commentLiterals.emplace(std::string(spelling)).first;
}

auto Control::memoryLayout() const -> MemoryLayout* { return d->memoryLayout; }

auto Control::newAnonymousId(std::string_view base) -> const Identifier* {
  auto id = std::string("$") + std::string(base) +
            std::to_string(++d->anonymousIdCount);
  return getIdentifier(id.c_str());
}

auto Control::getIdentifier(std::string_view name) -> const Identifier* {
  return &*d->identifiers.emplace(std::string(name)).first;
}

auto Control::getOperatorId(TokenKind op) -> const OperatorId* {
  return &*d->operatorIds.emplace(op).first;
}

auto Control::getDestructorId(const Name* name) -> const DestructorId* {
  return &*d->destructorIds.emplace(name).first;
}

auto Control::getLiteralOperatorId(std::string_view name)
    -> const LiteralOperatorId* {
  return &*d->literalOperatorIds.emplace(std::string(name)).first;
}

auto Control::getConversionFunctionId(const Type* type)
    -> const ConversionFunctionId* {
  return &*d->conversionFunctionIds.emplace(type).first;
}

auto Control::getTemplateId(const Name* name,
                            std::vector<TemplateArgument> arguments)
    -> const TemplateId* {
  return &*d->templateIds.emplace(name, std::move(arguments)).first;
}

auto Control::getVoidType() -> const VoidType* { return &d->voidType; }

auto Control::getNullptrType() -> const NullptrType* { return &d->nullptrType; }

auto Control::getDecltypeAutoType() -> const DecltypeAutoType* {
  return &d->decltypeAutoType;
}

auto Control::getAutoType() -> const AutoType* { return &d->autoType; }

auto Control::getBoolType() -> const BoolType* { return &d->boolType; }

auto Control::getSignedCharType() -> const SignedCharType* {
  return &d->signedCharType;
}

auto Control::getShortIntType() -> const ShortIntType* {
  return &d->shortIntType;
}

auto Control::getIntType() -> const IntType* { return &d->intType; }

auto Control::getLongIntType() -> const LongIntType* { return &d->longIntType; }

auto Control::getLongLongIntType() -> const LongLongIntType* {
  return &d->longLongIntType;
}

auto Control::getUnsignedCharType() -> const UnsignedCharType* {
  return &d->unsignedCharType;
}

auto Control::getUnsignedShortIntType() -> const UnsignedShortIntType* {
  return &d->unsignedShortIntType;
}

auto Control::getUnsignedIntType() -> const UnsignedIntType* {
  return &d->unsignedIntType;
}

auto Control::getUnsignedLongIntType() -> const UnsignedLongIntType* {
  return &d->unsignedLongIntType;
}

auto Control::getUnsignedLongLongIntType() -> const UnsignedLongLongIntType* {
  return &d->unsignedLongLongIntType;
}

auto Control::getCharType() -> const CharType* { return &d->charType; }

auto Control::getChar8Type() -> const Char8Type* { return &d->char8Type; }

auto Control::getChar16Type() -> const Char16Type* { return &d->char16Type; }

auto Control::getChar32Type() -> const Char32Type* { return &d->char32Type; }

auto Control::getWideCharType() -> const WideCharType* {
  return &d->wideCharType;
}

auto Control::getFloatType() -> const FloatType* { return &d->floatType; }

auto Control::getDoubleType() -> const DoubleType* { return &d->doubleType; }

auto Control::getLongDoubleType() -> const LongDoubleType* {
  return &d->longDoubleType;
}

auto Control::getClassDescriptionType() -> const ClassDescriptionType* {
  return &d->classDescriptionType;
}

auto Control::getQualType(const Type* elementType, CvQualifiers cvQualifiers)
    -> const QualType* {
  return &*d->qualTypes.emplace(elementType, cvQualifiers).first;
}

auto Control::getConstType(const Type* elementType) -> const QualType* {
  return getQualType(elementType, CvQualifiers::kConst);
}

auto Control::getVolatileType(const Type* elementType) -> const QualType* {
  return getQualType(elementType, CvQualifiers::kVolatile);
}

auto Control::getConstVolatileType(const Type* elementType) -> const QualType* {
  return getQualType(elementType, CvQualifiers::kConstVolatile);
}

auto Control::getBoundedArrayType(const Type* elementType, std::size_t size)
    -> const BoundedArrayType* {
  return &*d->boundedArrayTypes.emplace(elementType, size).first;
}

auto Control::getUnboundedArrayType(const Type* elementType)
    -> const UnboundedArrayType* {
  return &*d->unboundedArrayTypes.emplace(elementType).first;
}

auto Control::getPointerType(const Type* elementType) -> const PointerType* {
  return &*d->pointerTypes.emplace(elementType).first;
}

auto Control::getLvalueReferenceType(const Type* elementType)
    -> const LvalueReferenceType* {
  return &*d->lvalueReferenceTypes.emplace(elementType).first;
}

auto Control::getRvalueReferenceType(const Type* elementType)
    -> const RvalueReferenceType* {
  return &*d->rvalueReferenceTypes.emplace(elementType).first;
}

auto Control::getFunctionType(const Type* returnType,
                              std::vector<const Type*> parameterTypes,
                              bool isVariadic, CvQualifiers cvQualifiers,
                              RefQualifier refQualifier, bool isNoexcept)
    -> const FunctionType* {
  return &*d->functionTypes
               .emplace(returnType, std::move(parameterTypes), isVariadic,
                        cvQualifiers, refQualifier, isNoexcept)
               .first;
}

auto Control::getMemberObjectPointerType(const ClassType* classType,
                                         const Type* elementType)
    -> const MemberObjectPointerType* {
  return &*d->memberObjectPointerTypes.emplace(classType, elementType).first;
}

auto Control::getMemberFunctionPointerType(const ClassType* classType,
                                           const FunctionType* functionType)
    -> const MemberFunctionPointerType* {
  return &*d->memberFunctionPointerTypes.emplace(classType, functionType).first;
}

auto Control::getUnresolvedNameType(TranslationUnit* unit,
                                    NestedNameSpecifierAST* nestedNameSpecifier,
                                    UnqualifiedIdAST* unqualifiedId)
    -> const UnresolvedNameType* {
  return &*d->unresolvedNameTypes
               .emplace(unit, nestedNameSpecifier, unqualifiedId)
               .first;
}

auto Control::getUnresolvedBoundedArrayType(TranslationUnit* unit,
                                            const Type* elementType,
                                            ExpressionAST* sizeExpression)
    -> const UnresolvedBoundedArrayType* {
  return &*d->unresolvedBoundedArrayTypes
               .emplace(unit, elementType, sizeExpression)
               .first;
}

auto Control::getUnresolvedUnderlyingType(TranslationUnit* unit,
                                          TypeIdAST* typeId)
    -> const UnresolvedUnderlyingType* {
  return &*d->unresolvedUnderlyingTypes.emplace(unit, typeId).first;
}

auto Control::newClassType() -> const ClassType* {
  return &d->classTypes.emplace_front();
}

auto Control::newUnionType() -> const UnionType* {
  return &d->unionTypes.emplace_front();
}

auto Control::newNamespaceType() -> const NamespaceType* {
  return &d->namespaceTypes.emplace_front();
}

auto Control::newEnumType() -> const EnumType* {
  return &d->enumTypes.emplace_front();
}

auto Control::newScopedEnumType() -> const ScopedEnumType* {
  return &d->scopedEnumTypes.emplace_front();
}

auto Control::newNamespaceSymbol(Scope* enclosingScope) -> NamespaceSymbol* {
  auto symbol = &d->namespaceSymbols.emplace_front(enclosingScope);
  auto type = newNamespaceType();
  symbol->setType(type);
  type->setSymbol(symbol);
  return symbol;
}

auto Control::newConceptSymbol(Scope* enclosingScope) -> ConceptSymbol* {
  auto symbol = &d->conceptSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newClassSymbol(Scope* enclosingScope) -> ClassSymbol* {
  auto symbol = &d->classSymbols.emplace_front(enclosingScope);
  auto type = newClassType();
  symbol->setType(type);
  type->setSymbol(symbol);
  return symbol;
}

auto Control::newUnionSymbol(Scope* enclosingScope) -> UnionSymbol* {
  auto symbol = &d->unionSymbols.emplace_front(enclosingScope);
  auto type = newUnionType();
  symbol->setType(type);
  type->setSymbol(symbol);
  return symbol;
}

auto Control::newEnumSymbol(Scope* enclosingScope) -> EnumSymbol* {
  auto symbol = &d->enumSymbols.emplace_front(enclosingScope);
  auto type = newEnumType();
  symbol->setType(type);
  type->setSymbol(symbol);
  return symbol;
}

auto Control::newScopedEnumSymbol(Scope* enclosingScope) -> ScopedEnumSymbol* {
  auto symbol = &d->scopedEnumSymbols.emplace_front(enclosingScope);
  auto type = newScopedEnumType();
  symbol->setType(type);
  type->setSymbol(symbol);
  return symbol;
}

auto Control::newFunctionSymbol(Scope* enclosingScope) -> FunctionSymbol* {
  auto symbol = &d->functionSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newLambdaSymbol(Scope* enclosingScope) -> LambdaSymbol* {
  auto symbol = &d->lambdaSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newFunctionParametersSymbol(Scope* enclosingScope)
    -> FunctionParametersSymbol* {
  auto symbol = &d->functionParametersSymbol.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newTemplateParametersSymbol(Scope* enclosingScope)
    -> TemplateParametersSymbol* {
  auto symbol = &d->templateParametersSymbol.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newBlockSymbol(Scope* enclosingScope) -> BlockSymbol* {
  auto symbol = &d->blockSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newTypeAliasSymbol(Scope* enclosingScope) -> TypeAliasSymbol* {
  auto symbol = &d->typeAliasSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newVariableSymbol(Scope* enclosingScope) -> VariableSymbol* {
  auto symbol = &d->variableSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newFieldSymbol(Scope* enclosingScope) -> FieldSymbol* {
  auto symbol = &d->fieldSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newParameterSymbol(Scope* enclosingScope) -> ParameterSymbol* {
  auto symbol = &d->parameterSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newTypeParameterSymbol(Scope* enclosingScope)
    -> TypeParameterSymbol* {
  auto symbol = &d->typeParameterSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newNonTypeParameterSymbol(Scope* enclosingScope)
    -> NonTypeParameterSymbol* {
  auto symbol = &d->nonTypeParameterSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newTemplateTypeParameterSymbol(Scope* enclosingScope)
    -> TemplateTypeParameterSymbol* {
  auto symbol = &d->templateTypeParameterSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newConstraintTypeParameterSymbol(Scope* enclosingScope)
    -> ConstraintTypeParameterSymbol* {
  auto symbol =
      &d->constraintTypeParameterSymbols.emplace_front(enclosingScope);
  return symbol;
}

auto Control::newEnumeratorSymbol(Scope* enclosingScope) -> EnumeratorSymbol* {
  auto symbol = &d->enumeratorSymbols.emplace_front(enclosingScope);
  return symbol;
}

}  // namespace cxx