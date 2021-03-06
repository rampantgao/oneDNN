/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "gpu/ocl/ref_resampling.hpp"

namespace dnnl {
namespace impl {
namespace gpu {
namespace ocl {

status_t ref_resampling_fwd_t::execute_forward(const exec_ctx_t &ctx) const {
    auto *compute_stream
            = utils::downcast<compute::compute_stream_t *>(ctx.stream());

    auto &src = CTX_IN_STORAGE(DNNL_ARG_SRC);
    auto &dst = CTX_OUT_STORAGE(DNNL_ARG_DST);
    compute::kernel_arg_list_t arg_list;
    arg_list.set(0, src);
    arg_list.set(1, dst);

    auto nd_range = pd()->dispatch.nd_range();
    const auto &pr = ctx.get_resource_mapper()->get<ocl_resource_t>(this);
    const auto &kernel = pr->get_kernel(binary_.get_id());

    status_t status = compute_stream->parallel_for(nd_range, kernel, arg_list);
    return status;
}

status_t ref_resampling_bwd_t::execute_backward(const exec_ctx_t &ctx) const {
    auto &diff_dst = CTX_IN_STORAGE(DNNL_ARG_DIFF_DST);
    auto &diff_src = CTX_OUT_STORAGE(DNNL_ARG_DIFF_SRC);

    compute::kernel_arg_list_t arg_list;
    arg_list.set(0, diff_src);
    arg_list.set(1, diff_dst);

    auto nd_range = pd()->dispatch.nd_range();
    auto *compute_stream
            = utils::downcast<compute::compute_stream_t *>(ctx.stream());
    const auto &pr = ctx.get_resource_mapper()->get<ocl_resource_t>(this);
    const auto &kernel = pr->get_kernel(binary_.get_id());

    status_t status = compute_stream->parallel_for(nd_range, kernel, arg_list);
    return status;
}

} // namespace ocl
} // namespace gpu
} // namespace impl
} // namespace dnnl
