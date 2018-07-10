/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

#ifndef ANALYSIS_IFDS_IDE_FLOW_FUNC_LAMBDA_H_
#define ANALYSIS_IFDS_IDE_FLOW_FUNC_LAMBDA_H_

#include <phasar/PhasarLLVM/IfdsIde/FlowFunction.h>
#include <functional>
#include <set>

namespace psr {

template <typename D>
class Lambda : public FlowFunction<D> {
 private:
  std::function<std::set<D>(D)> flow;

 public:
  Lambda(std::function<std::set<D>(D)> f) : flow(f) {}
  virtual ~Lambda() = default;
  std::set<D> computeTargets(D source) override { return flow(source); }
};

}  // namespace psr

#endif