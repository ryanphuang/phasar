/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/*
 * SpecialSummaries.h
 *
 *  Created on: 05.05.2017
 *      Author: philipp
 */

#ifndef SRC_ANALYSIS_IFDS_IDE_SPECIALSUMMARIES_H_
#define SRC_ANALYSIS_IFDS_IDE_SPECIALSUMMARIES_H_

#include <phasar/Config/Configuration.h>
#include <phasar/Config/ContainerConfiguration.h>
#include <phasar/PhasarLLVM/IfdsIde/EdgeFunction.h>
#include <phasar/PhasarLLVM/IfdsIde/EdgeFunctions/EdgeIdentity.h>
#include <phasar/PhasarLLVM/IfdsIde/FlowFunction.h>
#include <phasar/PhasarLLVM/IfdsIde/FlowFunctions/Identity.h>
#include <phasar/PhasarLLVM/Utils/BinaryDomain.h>
#include <phasar/Utils/IO.h>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

template <typename D, typename V = BinaryDomain>
class SpecialSummaries {
 private:
  map<string, shared_ptr<FlowFunction<D>>> SpecialFlowFunctions;
  map<string, shared_ptr<EdgeFunction<V>>> SpecialEdgeFunctions;
  vector<string> SpecialFunctionNames;

  // Constructs the SpecialSummaryMap such that it contains all glibc,
  // llvm.intrinsics and C++'s new, new[], delete, delete[] with identity
  // flow functions.
  SpecialSummaries() {
    string glibc = readFile(ConfigurationDirectory + GLIBCFunctionListFileName);
    vector<string> glibcfunctions = splitString(glibc, "\n");
    // Insert glibc function names
    SpecialFunctionNames.insert(SpecialFunctionNames.end(),
                                glibcfunctions.begin(), glibcfunctions.end());
    string llvmintrinsics =
        readFile(ConfigurationDirectory + LLVMIntrinsicFunctionListFileName);
    vector<string> llvmintrinsicfunctions = splitString(llvmintrinsics, "\n");
    // Insert llvm intrinsic function names
    SpecialFunctionNames.insert(SpecialFunctionNames.end(),
                                llvmintrinsicfunctions.begin(),
                                llvmintrinsicfunctions.end());
    // Insert allocation operators
    SpecialFunctionNames.insert(SpecialFunctionNames.end(),
                                {"_Znwm", "_Znam", "_ZdlPv", "_ZdaPv"});
    // insert default flow and edge functions
    for (auto &function_name : SpecialFunctionNames) {
      SpecialFlowFunctions.insert(
          std::make_pair(function_name, Identity<D>::v()));
      SpecialEdgeFunctions.insert(
          std::make_pair(function_name, EdgeIdentity<V>::v()));
    }
  }

 public:
  SpecialSummaries(const SpecialSummaries &) = delete;
  SpecialSummaries &operator=(const SpecialSummaries &) = delete;
  SpecialSummaries(SpecialSummaries &&) = delete;
  SpecialSummaries &operator=(SpecialSummaries &&) = delete;
  ~SpecialSummaries() = default;

  static SpecialSummaries<D, V> &getInstance() {
    static SpecialSummaries<D, V> instance;
    return instance;
  }

  // Returns true, when an existing function is overwritten, false otherwise.
  bool provideSpecialSummary(const string &name,
                             shared_ptr<FlowFunction<D>> flowfunction) {
    bool Override = containsSpecialSummary(name);
    SpecialFlowFunctions[name] = flowfunction;
    return Override;
  }

  // Returns true, when an existing function is overwritten, false otherwise.
  bool provideSpecialSummary(const string &name,
                             shared_ptr<FlowFunction<D>> flowfunction,
                             shared_ptr<EdgeFunction<V>> edgefunction) {
    bool Override = containsSpecialSummary(name);
    SpecialFlowFunctions[name] = flowfunction;
    SpecialEdgeFunctions[name] = edgefunction;
    return Override;
  }

  bool containsSpecialSummary(const llvm::Function *function) {
    return containsSpecialSummary(function->getName().str());
  }

  bool containsSpecialSummary(const string &name) {
    return SpecialFlowFunctions.count(name);
  }

  shared_ptr<FlowFunction<D>> getSpecialFlowFunctionSummary(
      const llvm::Function *function) {
    return getSpecialFlowFunctionSummary(function->getName().str());
  }

  shared_ptr<FlowFunction<D>> getSpecialFlowFunctionSummary(
      const string &name) {
    return SpecialFlowFunctions[name];
  }

  shared_ptr<EdgeFunction<V>> getSpecialEdgeFunctionSummary(
      const llvm::Function *function) {
    return getSpecialEdgeFunctionSummary(function->getName().str());
  }

  shared_ptr<EdgeFunction<V>> getSpecialEdgeFunctionSummary(
      const string &name) {
    return SpecialEdgeFunctions[name];
  }

  friend ostream &operator<<(ostream &os, const SpecialSummaries<D> &ss) {
    os << "SpecialSummaries:\n";
    for (auto &entry : ss.SpecialFunctionNames) {
      os << entry.first << " ";
    }
    return os;
  }
};

#endif /* SRC_ANALYSIS_IFDS_IDE_SPECIALSUMMARIES_HH_ */