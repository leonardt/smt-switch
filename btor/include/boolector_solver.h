/*********************                                                        */
/*! \file boolector_solver.h
** \verbatim
** Top contributors (to current version):
**   Makai Mann
** This file is part of the smt-switch project.
** Copyright (c) 2020 by the authors listed in the file AUTHORS
** in the top-level source directory) and their institutional affiliations.
** All rights reserved.  See the file LICENSE in the top-level source
** directory for licensing information.\endverbatim
**
** \brief Boolector implementation of AbsSmtSolver
**
**
**/

#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "boolector_extensions.h"
#include "boolector_sort.h"
#include "boolector_term.h"

#include "exceptions.h"
#include "result.h"
#include "smt.h"
#include "sort.h"

namespace smt {
/**
   Boolector Solver
 */
class BoolectorSolver : public AbsSmtSolver
{
 public:
  // might have to use std::unique_ptr<Btor>(boolector_new) and move it?
  BoolectorSolver() : AbsSmtSolver(BTOR), btor(boolector_new())
  {
    // set termination function -- throw an exception
    auto throw_exception = [](const char * msg) -> void {
      throw InternalSolverException(msg);
    };
    boolector_set_abort(throw_exception);
  };
  BoolectorSolver(const BoolectorSolver &) = delete;
  BoolectorSolver & operator=(const BoolectorSolver &) = delete;
  ~BoolectorSolver()
  {
    boolector_delete(btor);
  };
  void set_opt(const std::string option, const std::string value) override;
  void set_logic(const std::string logic) override;
  void assert_formula(const Term & t) override;
  Result check_sat() override;
  Result check_sat_assuming(const TermVec & assumptions) override;
  void push(uint64_t num = 1) override;
  void pop(uint64_t num = 1) override;
  Term get_value(const Term & t) const override;
  UnorderedTermMap get_array_values(const Term & arr,
                                    Term & out_const_base) const override;
  void get_unsat_core(UnorderedTermSet & out) override;
  Sort make_sort(const std::string name, uint64_t arity) const override;
  Sort make_sort(SortKind sk) const override;
  Sort make_sort(SortKind sk, uint64_t size) const override;
  Sort make_sort(SortKind sk, const Sort & sort1) const override;
  Sort make_sort(SortKind sk,
                 const Sort & sort1,
                 const Sort & sort2) const override;
  Sort make_sort(SortKind sk,
                 const Sort & sort1,
                 const Sort & sort2,
                 const Sort & sort3) const override;
  Sort make_sort(SortKind sk, const SortVec & sorts) const override;
  Sort make_sort(const Sort & sort_con, const SortVec & sorts) const override;
  Sort make_sort(const DatatypeDecl & d) const override;

  DatatypeDecl make_datatype_decl(const std::string & s) override;
  DatatypeConstructorDecl make_datatype_constructor_decl(
      const std::string s) override;
  void add_constructor(DatatypeDecl & dt, const DatatypeConstructorDecl & con) const override;
  void add_selector(DatatypeConstructorDecl & dt, const std::string & name, const Sort & s) const override;
  void add_selector_self(DatatypeConstructorDecl & dt, const std::string & name) const override;
  Term get_constructor(const Sort & s, std::string name) const override;
  Term get_tester(const Sort & s, std::string name) const override;
  Term get_selector(const Sort & s, std::string con, std::string name) const override;

  Term make_term(bool b) const override;
  Term make_term(int64_t i, const Sort & sort) const override;
  Term make_term(const std::string val,
                 const Sort & sort,
                 uint64_t base = 10) const override;
  Term make_term(const Term & val, const Sort & sort) const override;
  Term make_symbol(const std::string name, const Sort & sort) override;
  Term make_param(const std::string name, const Sort & sort) override;
  /* build a new term */
  Term make_term(Op op, const Term & t) const override;
  Term make_term(Op op, const Term & t0, const Term & t1) const override;
  Term make_term(Op op,
                 const Term & t0,
                 const Term & t1,
                 const Term & t2) const override;
  Term make_term(Op op, const TermVec & terms) const override;
  void reset() override;
  void reset_assertions() override;
  Term substitute(const Term term,
                  const UnorderedTermMap & substitution_map) const override;
  // helper methods for making a term with a primitive op
  Term apply_prim_op(PrimOp op, Term t) const;
  Term apply_prim_op(PrimOp op, Term t0, Term t1) const;
  Term apply_prim_op(PrimOp op, Term t0, Term t1, Term t2) const;
  Term apply_prim_op(PrimOp op, TermVec terms) const;
  void dump_smt2(std::string filename) const override;

 protected:
  Btor * btor;
  // store the names of created symbols
  std::unordered_set<std::string> symbol_names;

  bool base_context_1 = false;
  ///< if set to true, do all solving at context 1 in the solver
  ///< this then supports reset_assertions by popping and re-pushing
  ///< the context. Without it, boolector does not support
  ///< reset_assertions yet
  ///< set this flag with set_opt("base-context-1", "true")
  size_t context_level = 0;  ///< tracks the current solving context level
};
}  // namespace smt

