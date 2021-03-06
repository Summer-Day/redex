/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <boost/optional.hpp>

#include "DexClass.h"

struct Location;

namespace dispatch {

enum Type {
  // Ctor that takes the generated type tag as the last parameter.
  CTOR_WITH_TYPE_TAG_PARAM,
  CTOR,
  STATIC,
  VIRTUAL,
  DIRECT,
  OTHER_TYPE
};

struct Spec {
  DexType* owner_type;
  Type type;
  const std::string name;
  DexProto* proto;
  DexAccessFlags access_flags;
  DexField* type_tag_field;
  DexMethod* overridden_meth;
  boost::optional<size_t> max_num_dispatch_target;
  bool keep_debug_info;

  Spec(DexType* owner_type,
       Type type,
       const std::string name,
       DexProto* proto,
       DexAccessFlags access_flags,
       DexField* type_tag_field,
       DexMethod* overridden_meth,
       bool keep_debug_info)
      : owner_type(owner_type),
        type(type),
        name(name),
        proto(proto),
        access_flags(access_flags),
        type_tag_field(type_tag_field),
        overridden_meth(overridden_meth),
        keep_debug_info(keep_debug_info) {
    max_num_dispatch_target = boost::none;
  }

  Spec(DexType* owner_type,
       Type type,
       const std::string name,
       DexProto* proto,
       DexAccessFlags access_flags,
       DexField* type_tag_field,
       DexMethod* overridden_meth,
       boost::optional<size_t> max_num_dispatch_target,
       bool keep_debug_info)
      : owner_type(owner_type),
        type(type),
        name(name),
        proto(proto),
        access_flags(access_flags),
        type_tag_field(type_tag_field),
        overridden_meth(overridden_meth),
        max_num_dispatch_target(max_num_dispatch_target),
        keep_debug_info(keep_debug_info) {}
};

struct DispatchMethod {
  DexMethod* main_dispatch;
  std::vector<DexMethod*> sub_dispatches;

  explicit DispatchMethod(DexMethod* main) : main_dispatch(main) {}
  DispatchMethod(DexMethod* main, std::vector<DexMethod*> subs)
      : main_dispatch(main), sub_dispatches(subs) {}
};

/**
 * A high level API that assesses the size of the switch dispatch needed and
 * potentially split it when necessary.
 */
dispatch::DispatchMethod create_virtual_dispatch(
    const Spec& spec,
    const std::map<SwitchIndices, DexMethod*>& indices_to_callee);

DexMethod* create_ctor_or_static_dispatch(
    const Spec& spec,
    const std::map<SwitchIndices, DexMethod*>& indices_to_callee);

/**
 * Create a simple dispatch for the methods who should have the same proto and
 * same `this` type if the methods are virtual or direct. Methods should all
 * be direct, static or virtual, do not support constructors or mix.
 */
DexMethod* create_simple_dispatch(
    const std::map<SwitchIndices, DexMethod*>& indices_to_callee,
    DexAnnotationSet* anno = nullptr,
    bool with_debug_item = false);

/**
 * Generate a new dispatch method name.
 */
DexString* gen_dispatch_name(DexType* owner,
                             DexProto* proto,
                             std::string orig_name);

/**
 * If the method's name starts with DISPATH_PREFIX and contains a switch
 * instruction or some conditional branches, it may be an dispatch method.
 * This is only used in method-merger service.
 */
bool may_be_dispatch(const DexMethod* method);
} // namespace dispatch
