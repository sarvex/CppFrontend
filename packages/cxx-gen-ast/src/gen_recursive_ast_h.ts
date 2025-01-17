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

import { groupNodesByBaseType } from "./groupNodesByBaseType.js";
import { AST } from "./parseAST.js";
import { cpy_header } from "./cpy_header.js";
import * as fs from "fs";

export function gen_recursive_ast_h({
  ast,
  output,
}: {
  ast: AST;
  output: string;
}) {
  const code: string[] = [];
  const emit = (line = "") => code.push(line);

  const by_base = groupNodesByBaseType(ast);

  emit(`class RecursiveASTVisitor : public ASTVisitor {`);
  emit(`public:`);
  emit(`  void accept(AST* ast);`);
  emit();

  const funcName = (base: string) =>
    "accept" + base[0].toUpperCase() + base.slice(1, -3);

  const types = new Set<string>();

  ast.nodes.forEach((node) => {
    node.members.forEach((m) => {
      if (m.kind === "node" || m.kind === "node-list") types.add(m.type);
    });
  });

  Array.from(types.values()).forEach((base) => {
    emit(`virtual void ${funcName(base)}(${base}* ast);`);
  });

  emit(`virtual auto preVisit(AST*) -> bool { return true; }`);
  emit(`virtual void postVisit(AST*) {}`);

  by_base.forEach((nodes) => {
    if (!Array.isArray(nodes)) throw new Error("not an array");
    emit();
    nodes.forEach(({ name }) => {
      emit(`  void visit(${name}* ast) override;`);
    });
  });

  emit(`};`);

  const out = `${cpy_header}
#pragma once

#include <cxx/ast_visitor.h>

namespace cxx {

${code.join("\n")}

} // namespace cxx
`;

  fs.writeFileSync(output, out);
}
