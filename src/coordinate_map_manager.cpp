/*
 * Copyright (c) 2020 NVIDIA CORPORATION.
 * Copyright (c) Chris Choy (chrischoy@ai.stanford.edu).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Please cite "4D Spatio-Temporal ConvNets: Minkowski Convolutional Neural
 * Networks", CVPR'19 (https://arxiv.org/abs/1904.08755) if you use any part
 * of the code.
 */
#include "coordinate_map_manager.hpp"
#include "coordinate_map_key.hpp"
#include "errors.hpp"
#include "kernel_region.hpp"
#include "utils.hpp"

#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

namespace minkowski {

/*

template <typename MapType>
vector<at::Tensor>
CoordsManager<MapType>::getCoordsMap(py::object py_in_coords_key,
                                     py::object py_out_coords_key) const {
  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();
  const uint64_t in_coords_key = p_in_coords_key->getKey();
  const uint64_t out_coords_key = p_out_coords_key->getKey();

  const auto in_map_iter = coords_maps.find(in_coords_key);
  const auto out_map_iter = coords_maps.find(out_coords_key);

  ASSERT(in_map_iter != coords_maps.end(), "Input coords not found at",
         to_string(in_coords_key));
  ASSERT(out_map_iter != coords_maps.end(), "Output coords not found at",
         to_string(out_coords_key));

  const auto &out_tensor_strides = p_out_coords_key->getTensorStride();
  const auto in_out =
      in_map_iter->second.stride_map(out_map_iter->second, out_tensor_strides);

  const auto &ins = in_out.first;
  const auto &outs = in_out.second;
  // All size
  const auto N = std::accumulate(ins.begin(), ins.end(), 0,
                                 [](size_t curr_sum, const vector<int> &map) {
                                   return curr_sum + map.size();
                                 });

  at::Tensor in_out_1 =
      torch::empty({N}, torch::TensorOptions().dtype(torch::kInt64));
  at::Tensor in_out_2 =
      torch::empty({N}, torch::TensorOptions().dtype(torch::kInt64));

  auto a_in_out_1 = in_out_1.accessor<long int, 1>();
  auto a_in_out_2 = in_out_2.accessor<long int, 1>();

  size_t curr_it = 0;
  for (const auto &in : ins)
    for (const auto i : in)
      a_in_out_1[curr_it++] = i;

  curr_it = 0;
  for (const auto &out : outs)
    for (const auto o : out)
      a_in_out_2[curr_it++] = o;

  return {in_out_1, in_out_2};
}

// Generate and return the ins -> out map.
template <typename MapType>
pair<vector<at::Tensor>, vector<at::Tensor>>
CoordsManager<MapType>::getUnionMap(vector<py::object> py_in_coords_keys,
                                    py::object py_out_coords_key) {

  // all exception handling will be done inside the following
  const InOutMapsRefPair<int> in_outs =
      getUnionInOutMaps(py_in_coords_keys, py_out_coords_key);
  const auto &ins = in_outs.first;
  const auto &outs = in_outs.second;

  // Size of the in out maps
  const auto N = ins.size();

  // Return torch tensor
  vector<at::Tensor> th_ins;
  vector<at::Tensor> th_outs;
  for (size_t i = 0; i < N; ++i) {
    at::Tensor th_in = torch::empty(
        {(long)ins[i].size()}, torch::TensorOptions().dtype(torch::kInt64));
    at::Tensor th_out = torch::empty(
        {(long)outs[i].size()}, torch::TensorOptions().dtype(torch::kInt64));

    copy_types(ins[i], th_in);
    copy_types(outs[i], th_out);

    th_ins.push_back(move(th_in));
    th_outs.push_back(move(th_out));
  }

  return make_pair(th_ins, th_outs);
}

template <typename MapType>
uint64_t
CoordsManager<MapType>::getCoordsKey(const vector<int> &tensor_strides) const {
  auto tensor_stride_hash = hash_vec<vector<int>>(tensor_strides);
  ASSERT(coords_maps.find(tensor_stride_hash) != coords_maps.end(),
         "The coord map doesn't exist for the given tensor strides ",
         "tensor_stride: ", ArrToString(tensor_strides));
  return tensor_stride_hash;
}

template <typename MapType>
void CoordsManager<MapType>::setOriginCoordsKey(py::object py_coords_key) {
  CoordsKey *p_coords_key = py_coords_key.cast<CoordsKey *>();
  const int D = p_coords_key->getDimension();
  ASSERT(D > 0, "Invalid dimension: ", D);
  if (!p_coords_key->key_set) {
    p_coords_key->setKey(createOriginCoords(D));
    const vector<int> zero_vec(D, 0);
    p_coords_key->setTensorStride(zero_vec);
  } else {
    auto coords_key = p_coords_key->getKey();
    auto origin_key = createOriginCoords(D);
    ASSERT(coords_key == origin_key, "Invalid key: ", to_string(coords_key),
           " != Origin key: ", to_string(origin_key));
  }
}
*/

/*******************************
 * Initialization
 *******************************/

namespace detail {

template <typename coordinate_type>
struct insert_and_map_functor<coordinate_type, std::allocator,
                              CoordinateMapCPU> {

  std::pair<at::Tensor, at::Tensor> operator()(
      coordinate_map_key_type &map_key, at::Tensor const &th_coordinate,
      CoordinateMapManager<coordinate_type, std::allocator, CoordinateMapCPU>
          &manager) {
    LOG_DEBUG("initialize_and_map");
    uint32_t const N = th_coordinate.size(0);
    uint32_t const coordinate_size = th_coordinate.size(1);
    coordinate_type *p_coordinate = th_coordinate.data_ptr<coordinate_type>();
    auto map = CoordinateMapCPU<coordinate_type, std::allocator>(
        N, coordinate_size, map_key.first);
    auto map_inverse_map = map.template insert_and_map<true>(
        p_coordinate, p_coordinate + N * coordinate_size);
    LOG_DEBUG("mapping size:", map_inverse_map.first.size());

    // insert moves map
    manager.insert(map_key, map);

    auto const &mapping = map_inverse_map.first;
    auto const &inverse_mapping = map_inverse_map.second;

    // return tensors
    at::Tensor th_mapping = torch::empty(
        {(int64_t)mapping.size()},
        torch::TensorOptions().requires_grad(false).dtype(torch::kInt64));
    at::Tensor th_inverse_mapping = torch::empty(
        {(int64_t)inverse_mapping.size()},
        torch::TensorOptions().requires_grad(false).dtype(torch::kInt64));

    // copy_n to int to long
    int64_t *p_mapping = th_mapping.data_ptr<int64_t>();
    for (default_types::index_type i = 0; i < mapping.size(); ++i) {
      p_mapping[i] = mapping[i];
    }

    int64_t *p_inverse_mapping = th_inverse_mapping.data_ptr<int64_t>();
    for (default_types::index_type i = 0; i < inverse_mapping.size(); ++i) {
      p_inverse_mapping[i] = inverse_mapping[i];
    }

    return std::make_pair(std::move(th_mapping), std::move(th_inverse_mapping));
  }
};

} // namespace detail

/*
 * coords: coordinates in IntTensor
 * mapping: output mapping in IntTensor
 * tensor_strides: current tensor strides this coords will be initializeds
 * force_creation: even when there's a duplicate coords with the same tensor
 *                 strides.
 * force_remap: if there's duplicate coords, remap
 * allow_duplicate_coords: create map when there are duplicates in the
 * coordinates
 */
template <typename coordinate_type,
          template <typename C> class TemplatedAllocator,
          template <typename T, template <typename Q> class A>
          class CoordinateMapType>
std::pair<py::object, std::pair<at::Tensor, at::Tensor>>
CoordinateMapManager<coordinate_type, TemplatedAllocator, CoordinateMapType>::
    insert_and_map(at::Tensor const &coordinate,
                   default_types::stride_type const tensor_stride,
                   std::string const string_id) {

  torch::TensorArg arg_coordinate(coordinate, "coordinates", 0);
  torch::CheckedFrom c = "initialize";
  torch::checkContiguous(c, arg_coordinate);
  // must match coordinate_type
  torch::checkScalarType(c, arg_coordinate, torch::kInt);
  torch::checkBackend(c, arg_coordinate.tensor,
                      detail::is_cpu_coordinate_map<CoordinateMapType>::value
                          ? torch::Backend::CPU
                          : torch::Backend::CUDA);
  torch::checkDim(c, arg_coordinate, 2);

  auto const coordinate_size = (index_type)coordinate.size(1);

  // Basic assertions
  ASSERT(coordinate_size - 1 == tensor_stride.size(),
         "The coordinate dimension (coordinate_size - 1):", coordinate_size - 1,
         " must match the size of tensor stride:", ArrToString(tensor_stride));

  // generate the map_key
  coordinate_map_key_type map_key = std::make_pair(tensor_stride, string_id);
  if (m_coordinate_maps.find(map_key) != m_coordinate_maps.end()) {
    WARNING(true, "CoordinateMapKey collision detected:", map_key,
            "generating new string id.");
    map_key = get_random_string_id(tensor_stride, string_id);
  }

  LOG_DEBUG("initializing a map with tensor stride:", map_key.first,
            "string id:", map_key.second);
  // Create the concurrent coords map
  auto const map_inverse_map =
      detail::insert_and_map_functor<coordinate_type, TemplatedAllocator,
                                     CoordinateMapType>()(map_key, coordinate,
                                                          *this);


  py::object py_key = py::cast(new CoordinateMapKey(coordinate_size, map_key));

  return std::make_pair(py_key, std::move(map_inverse_map));
}

// stride
template <typename coordinate_type,
          template <typename C> class TemplatedAllocator,
          template <typename T, template <typename Q> class A>
          class CoordinateMapType>
std::pair<coordinate_map_key_type, bool>
CoordinateMapManager<coordinate_type, TemplatedAllocator, CoordinateMapType>::
    stride(coordinate_map_key_type const &in_map_key,
           stride_type const &kernel_stride) {
  ASSERT(exists(in_map_key), ERROR_MAP_NOT_FOUND);
  // check if the key exists.
  LOG_DEBUG("In tensor stride:", in_map_key.first,
            "kernel stride:", kernel_stride);
  coordinate_map_key_type out_map_key(
      detail::stride_tensor_stride(in_map_key.first, kernel_stride, false), "");
  LOG_DEBUG("Out stride map key:", out_map_key);
  bool const exists_out_map = exists(out_map_key);
  if (!exists_out_map) {
    // operator[] required mapped_type(), which is not defined.
    // ASSERTION already checked that in_map_key exists.
    map_type const &in_map = m_coordinate_maps.find(in_map_key)->second;
    map_type out_map = in_map.stride(kernel_stride);
    insert(out_map_key, out_map);
  }
  // (key, new map generated flag)
  return std::make_pair(out_map_key, !exists_out_map);
}

template <typename coordinate_type,
          template <typename C> class TemplatedAllocator,
          template <typename T, template <typename Q> class A>
          class CoordinateMapType>
std::pair<coordinate_map_key_type, bool>
CoordinateMapManager<coordinate_type, TemplatedAllocator, CoordinateMapType>::
    stride_region(coordinate_map_key_type const &in_map_key,
                  cpu_kernel_region<coordinate_type> &kernel,
                  bool is_transpose) {
  ASSERT(exists(in_map_key), ERROR_MAP_NOT_FOUND);
  // check if the key exists.
  stride_type out_tensor_stride(kernel.tensor_stride(),
                                kernel.tensor_stride() +
                                    kernel.coordinate_size() - 1);
  coordinate_map_key_type out_map_key(out_tensor_stride, "");
  bool const exists_out_map = exists(out_map_key);
  if (!exists_out_map) {
    ASSERT(false, ERROR_NOT_IMPLEMENTED);
    // operator[] required mapped_type(), which is not defined.
    // ASSERTION already checked that in_map_key exists.
    //
    // map_type const &in_map = m_coordinate_maps.find(in_map_key)->second;
    // map_type out_map = in_map.stride_region(kernel_region);
    // insert(out_map_key, out_map);
  }
  // (key, new map generated flag)
  return std::make_pair(out_map_key, !exists_out_map);
}

// Kernel map

namespace detail {

template <typename coordinate_type>
struct kernel_map_functor<coordinate_type, std::allocator, CoordinateMapCPU,
                          cpu_kernel_map> {

  cpu_kernel_map
  operator()(CoordinateMapCPU<coordinate_type, std::allocator> const &in_map,
             CoordinateMapCPU<coordinate_type, std::allocator> const &out_map,
             CUDAKernelMapMode::Mode kernel_map_mode,
             cpu_kernel_region<coordinate_type> &kernel) {
    return in_map.kernel_map(out_map, kernel);
  }
};

template <typename coordinate_type>
struct stride_map_functor<coordinate_type, std::allocator, CoordinateMapCPU,
                          cpu_kernel_map> {

  cpu_kernel_map
  operator()(CoordinateMapCPU<coordinate_type, std::allocator> const &in_map,
             CoordinateMapCPU<coordinate_type, std::allocator> const &out_map,
             default_types::stride_type const &stride) {
    return in_map.stride_map(out_map, stride);
  }
};

// a partial specialization functor for kernel map in/out swap
template <> struct swap_in_out_map_functor<cpu_kernel_map> {

  cpu_kernel_map operator()(cpu_kernel_map const &kernel_map) {
    return std::make_pair(kernel_map.second, kernel_map.first);
  }
};

} // namespace detail

/*
 * Given tensor_stride_src and tensor_stride_dst, find the respective coord_maps
 * and return the indices of the coord_map_ind in coord_map_dst
 */
template <typename coordinate_type,
          template <typename C> class TemplatedAllocator,
          template <typename T, template <typename Q> class A>
          class CoordinateMapType>
typename CoordinateMapManager<coordinate_type, TemplatedAllocator,
                              CoordinateMapType>::kernel_map_type const &
CoordinateMapManager<coordinate_type, TemplatedAllocator, CoordinateMapType>::
    kernel_map(CoordinateMapKey const *p_in_map_key,
               CoordinateMapKey const *p_out_map_key,
               stride_type const &kernel_size, //
               stride_type const &kernel_stride,
               stride_type const &kernel_dilation,
               RegionType::Type const region_type, at::Tensor const &offset,
               bool is_transpose, bool is_pool) {
  ASSERT(region_type != RegionType::CUSTOM, "Not implemented yet.");
  if (region_type == RegionType::CUSTOM)
    ASSERT(offset.is_cuda() ==
               !detail::is_cpu_coordinate_map<CoordinateMapType>::value,
           "Invalid device for offset");

  size_type kernel_dim = kernel_size.size();

  ASSERT(kernel_dim == kernel_stride.size(), "kernel size mismatch");
  ASSERT(kernel_dim == kernel_dilation.size(), "kernel size mismatch");

  // in_coords_key->tensor_stride * kernel_stride ==
  // out_coords_key->tensor_stride

  kernel_map_key_type const kernel_map_key =
      std::make_tuple(p_in_map_key->get_key(), p_out_map_key->get_key(), // maps
                      kernel_size, kernel_stride, kernel_dilation, // kernels
                      region_type, is_transpose, is_pool);

  const auto &kernel_map_iter = m_kernel_maps.find(kernel_map_key);
  LOG_DEBUG("kernel map key set");

  if (kernel_map_iter == m_kernel_maps.end()) {
    // create a kernel map if it exists
    auto const in_map_it = m_coordinate_maps.find(p_in_map_key->get_key());
    auto const out_map_it = m_coordinate_maps.find(p_out_map_key->get_key());

    ASSERT(in_map_it != m_coordinate_maps.end(), "in_map", ERROR_MAP_NOT_FOUND);
    ASSERT(out_map_it != m_coordinate_maps.end(), "out_map",
           ERROR_MAP_NOT_FOUND);

    auto const &in_map = in_map_it->second;
    auto const &out_map = out_map_it->second;

    auto const D = in_map.coordinate_size();
    LOG_DEBUG("coordinate_size:", D,
              "tensor_stride:", in_map.get_tensor_stride());

    // +1 for batch index
    ASSERT(kernel_dim + 1 == in_map.coordinate_size(), "kernel size mismatch");
    ASSERT(kernel_dim + 1 == out_map.coordinate_size(), "kernel size mismatch");
    if (!is_transpose) {
      if (is_pool && (kernel_stride == kernel_size)) {
        LOG_DEBUG("generating stride_map");
        auto const stride_map =
            detail::stride_map_functor<coordinate_type, TemplatedAllocator,
                                       CoordinateMapType, kernel_map_type>()(
                in_map, out_map, out_map.get_tensor_stride());

        m_kernel_maps[kernel_map_key] = std::move(stride_map);

      } else {
        LOG_DEBUG("generating kernel map");

        // Default kernel map
        auto kernel_region = cpu_kernel_region<coordinate_type>(
            region_type,                       //
            in_map.coordinate_size(),          //
            in_map.get_tensor_stride().data(), //
            kernel_size.data(),                //
            kernel_dilation.data(),            //
            0, offset.data_ptr<coordinate_type>(), offset.size(0));

        auto const kernel_map =
            detail::kernel_map_functor<coordinate_type, TemplatedAllocator,
                                       CoordinateMapType, kernel_map_type>()(
                in_map, out_map, m_kernel_map_mode, kernel_region);

        LOG_DEBUG("kernel_map done");
        m_kernel_maps[kernel_map_key] = std::move(kernel_map);
        LOG_DEBUG("kernel_map saved");
      }
    } else { // is_transpose == true
      // Check first if the out2in kernel map exists
      //
      // Create temporary key for the flipped in/out
      kernel_map_key_type const swapped_kernel_map_key = std::make_tuple(
          p_out_map_key->get_key(), p_in_map_key->get_key(), // maps
          kernel_size, kernel_stride, kernel_dilation,       // kernels
          region_type, false, is_pool);

      // Check if the temporary key exists and return swapped in/out
      if (m_kernel_maps.find(swapped_kernel_map_key) != m_kernel_maps.end()) {
        // copy the in out maps from the existing maps
        m_kernel_maps[kernel_map_key] =
            detail::swap_in_out_map_functor<kernel_map_type>()(
                m_kernel_maps[swapped_kernel_map_key]);
      } else { // create in out kernel if it doesn't exist
        if (is_pool && kernel_stride == kernel_size) {
          // e.g. out_map has tensor stride 2 in_map has tensor stride 4.
          // Thus, create a stride map from 2 to 4, out to in.
          auto const stride_map =
              detail::stride_map_functor<coordinate_type, TemplatedAllocator,
                                         CoordinateMapType, kernel_map_type>()(
                  out_map, in_map, kernel_stride);

          // TODO Replace the kernel_map values to shared pointers.
          m_kernel_maps[kernel_map_key] =
              detail::swap_in_out_map_functor<kernel_map_type>()(stride_map);
        } else {
          // Default kernel map
          auto kernel_region = cpu_kernel_region<coordinate_type>(
              region_type,                        //
              out_map.coordinate_size(),          //
              out_map.get_tensor_stride().data(), //
              kernel_size.data(),                 //
              kernel_dilation.data(),             //
              0, offset.data_ptr<coordinate_type>(), offset.size(0));

          // out to in kernel map
          auto const kernel_map =
              detail::kernel_map_functor<coordinate_type, TemplatedAllocator,
                                         CoordinateMapType, kernel_map_type>()(
                  out_map, in_map, m_kernel_map_mode, kernel_region);

          LOG_DEBUG("kernel_map done");
          m_kernel_maps[kernel_map_key] =
              detail::swap_in_out_map_functor<kernel_map_type>()(
                  std::move(kernel_map));
          LOG_DEBUG("kernel_map saved");
        }
      }
    }
  }

  // TODO check if it copies or moves the internal data
  return m_kernel_maps[kernel_map_key];
}

/*********************************/
/*
template <typename MapType>
uint64_t
CoordsManager<MapType>::createPrunedCoords(at::Tensor use_feat,
                                           py::object py_in_coords_key,
                                           py::object py_out_coords_key) {
  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();
  const uint64_t in_coords_key = p_in_coords_key->getKey();

  ASSERT(existsCoordsKey(in_coords_key),
         "The coord map doesn't exist for the given coords_key: ",
         to_string(in_coords_key), ".");

  // set a random coords key
  const uint64_t out_coords_key = getRandomCoordsKey();

  // Set the pycoordskey
  p_out_coords_key->setDimension(p_in_coords_key->getDimension());
  p_out_coords_key->setKey(out_coords_key);
  if (!p_out_coords_key->tensor_stride_set)
    p_out_coords_key->setTensorStride(p_in_coords_key->getTensorStride());

  coords_maps[out_coords_key] =
      coords_maps[in_coords_key].prune(use_feat.data<bool>(), use_feat.size(0));

  return out_coords_key;
}

template <typename MapType>
uint64_t CoordsManager<MapType>::createOriginCoords(const int D) {
  const vector<int> zero_tensor_strides(D, 0);
  const uint64_t out_coords_key = hash_vec(zero_tensor_strides);
  // If the coordinates already exists, return the key.
  if (existsCoordsKey(out_coords_key))
    return out_coords_key;

  coords_maps[out_coords_key] = CoordsMap<MapType>(D + 1, batch_indices);
  return out_coords_key;
}

template <typename MapType>
uint64_t
CoordsManager<MapType>::createUnionCoords(vector<py::object> py_in_coords_keys,
                                          py::object py_out_coords_key) {
  vector<CoordsKey *> p_in_coords_keys;
  CoordsKey *p_in_coords_key = py_in_coords_keys[0].cast<CoordsKey *>();
  auto tensor_strides = p_in_coords_key->getTensorStride();
  for (const auto &py_in_coords_key : py_in_coords_keys) {
    // Set the tensor strides to the smallest elements.
    p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
    p_in_coords_keys.push_back(p_in_coords_key);
    transform(tensor_strides.begin(),                            // In1 begin
              tensor_strides.end(),                              // In1 end
              p_in_coords_key->getTensorStride().begin(),        // In2 begin
              tensor_strides.begin(),                            // out begin
              [](int a, int b) -> int { return std::min(a, b); } // binary op
    );
    const uint64_t in_coords_key = p_in_coords_key->getKey();
    ASSERT(existsCoordsKey(in_coords_key),
           "The coord map doesn't exist for the given coords_key: ",
           to_string(in_coords_key), ".");
  }
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();

  vector<reference_wrapper<CoordsMap<MapType>>> in_coords_maps;
  for (const CoordsKey *p_in_coords_key : p_in_coords_keys) {
    CoordsMap<MapType> &curr_map = coords_maps[p_in_coords_key->getKey()];
    in_coords_maps.push_back(ref(curr_map));
  }

  // set a random coords key
  const uint64_t out_coords_key = getRandomCoordsKey();

  // Set the pycoordskey using the last coords_key
  p_out_coords_key->setDimension(p_in_coords_key->getDimension());
  p_out_coords_key->setKey(out_coords_key);
  p_out_coords_key->setTensorStride(tensor_strides);

  coords_maps[out_coords_key] =
      CoordsMap<MapType>::union_coords(in_coords_maps);

  return out_coords_key;
}

template <typename MapType>
const InOutMapKey
CoordsManager<MapType>::getUnionMapHashKey(vector<py::object> py_in_coords_keys,
                                           py::object py_out_coords_key) const {
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();
  ASSERT(py_in_coords_keys.size() > 1, "Number of input coords must be > 1");
  vector<CoordsKey *> p_in_coords_keys;
  // We use sum of coords key (even with overflow, it will be unique with high
  // prob). We use sum to make the key invariant to the order of the keys.
  uint64_t sum_in_coords_key = 0;
  CoordsKey *p_in_coords_key = py_in_coords_keys[0].cast<CoordsKey *>();
  for (auto &py_in_coords_key : py_in_coords_keys) {
    p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
    const uint64_t in_coords_key = p_in_coords_key->getKey();
    ASSERT(existsCoordsKey(in_coords_key),
           "The coord map doesn't exist for the given coords_key: ",
           to_string(in_coords_key), ".");
    sum_in_coords_key += in_coords_key;
  }

  ASSERT(p_out_coords_key->key_set, "Key is not set. out_coords_key: ",
         to_string(p_out_coords_key->getKey()));

  const uint64_t out_coords_key = p_out_coords_key->getKey();
  const vector<int> zero_vec(p_in_coords_key->getDimension(), 0);
  const uint64_t zero_hash = hash_vec(zero_vec);
  InOutMapKey map_key = {sum_in_coords_key,
                         out_coords_key,
                         zero_hash,
                         zero_hash,
                         zero_hash,
                         0,
                         false,
                         true};
  return map_key;
}
*/
/**
 * Entry function for coords map generation and the associated kernel maps.
 */
/*
template <typename MapType>
const InOutMapsRefPair<int> CoordsManager<MapType>::getInOutMaps(
    const vector<int> &tensor_strides, const vector<int> &strides,
    const vector<int> &kernel_sizes, const vector<int> &dilations,
    int region_type, const at::Tensor &offsets, py::object py_in_coords_key,
    py::object py_out_coords_key, bool is_transpose, bool is_pool,
    bool force_creation) {
  const int D = tensor_strides.size();
  ASSERT(D == tensor_strides.size() and D == strides.size() and
             D == kernel_sizes.size() and D == dilations.size(),
         "Size mismatch. tensor_strides: ", tensor_strides.size(),
         ", strides: ", strides.size(), ", kernel_sizes: ", kernel_sizes.size(),
         ", dilations: ", dilations.size());
  ASSERT(std::all_of(tensor_strides.begin(), tensor_strides.end(),
                     [](int k) { return k > 0; }),
         "Invalid tensor_strides: ", ArrToString(tensor_strides),
         " Tensor strides must be positive integers.");

  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();
  const uint64_t in_coords_key = p_in_coords_key->getKey();
  uint64_t out_coords_key;

  // 1. Create output coordinates if it doesn't exist
  //
  // create a new output coordinates if it is transpose and gen new coords
  if (!p_out_coords_key->key_set || force_creation) {
    if (!is_transpose) {
      // Will return the in_coords_key if strides == 1.
      out_coords_key = createStridedCoords(
          p_in_coords_key->getKey(), tensor_strides, strides, force_creation);
    } else {
      // out_coords_key = createTransposedOutCoords(
      //     p_in_coords_key->getKey(), tensor_strides, strides, kernel_sizes,
      //     dilations, region_type, offsets, force_creation);
      out_coords_key = createTransposedStridedRegionCoords(
          p_in_coords_key->getKey(), tensor_strides, strides, kernel_sizes,
          dilations, region_type, offsets, force_creation);
    }
    p_out_coords_key->setKey(out_coords_key);
  } else {
    out_coords_key = p_out_coords_key->getKey();
  }

  // 2. Generate kernel map
  const InOutMapKey map_key = getMapHashKey(
      tensor_strides, strides, kernel_sizes, dilations, region_type,
      py_in_coords_key, py_out_coords_key, is_transpose, is_pool);

  CoordsMap<MapType> &in_map = coords_maps[in_coords_key];
  CoordsMap<MapType> &out_map = coords_maps[out_coords_key];

  // Create kernel maps
  if (!is_transpose) { // NON TRANSPOSE
    if (!p_out_coords_key->tensor_stride_set) {
      p_out_coords_key->setTensorStride(tensor_strides);
      p_out_coords_key->stride(strides);
    }
    // For non transpose case
    // make a kernel mapping. The kernel will be saved with the map_key.
    if (in_maps.find(map_key) == in_maps.end()) {
      const vector<int> out_tensor_strides = computeOutTensorStride(
          tensor_strides, strides, false // is_transpose
          );

      // Create kernel map using the region if it is not a pooling or if the
      // tensor stride is not equal to the kernel size and the region type is
      // not cubic.
      //
      // TODO: even numbered kernel size to use region_type 0
      if (is_pool && (strides == kernel_sizes)) {

        const auto in_out = in_map.stride_map(out_map, out_tensor_strides);
        in_maps[map_key] = move(in_out.first);
        out_maps[map_key] = move(in_out.second);

      } else {
        Region region =
            Region(tensor_strides, kernel_sizes, dilations, region_type,
                   offsets.data<int>(), offsets.size(0));

        const auto in_out = in_map.kernel_map(out_map, region);
        in_maps[map_key] = move(in_out.first);
        out_maps[map_key] = move(in_out.second);
      }
    }
    return make_pair(ref(in_maps[map_key]), ref(out_maps[map_key]));

  } else { // TRANSPOSE

    if (!p_out_coords_key->tensor_stride_set) {
      p_out_coords_key->setTensorStride(tensor_strides);
      p_out_coords_key->up_stride(strides);
    }

    // Create temporary key for the flipped in/out
    const InOutMapKey tmp_map_key = getMapHashKey(
        tensor_strides, strides, kernel_sizes, dilations, region_type,
        py_out_coords_key, py_in_coords_key, false, is_pool);

    // Check if the temporary key exists and return swapped in/out
    if (in_maps.find(tmp_map_key) != in_maps.end()) {
      // copy the in out maps from the existing maps
      in_maps[map_key] = out_maps[tmp_map_key];
      out_maps[map_key] = in_maps[tmp_map_key];

    } else { // create in out kernel if it doesn't exist

      if (is_pool && strides == kernel_sizes && region_type == 0) {
        // out tensor strides are smaller than in tensor strides for transpose
        auto in_tensor_strides = p_in_coords_key->getTensorStride();
        auto out_in = out_map.stride_map(in_map, in_tensor_strides);

        in_maps[map_key] = move(out_in.second);
        out_maps[map_key] = move(out_in.first);

      } else {
        // out tensor strides are smaller than in tensor strides for transpose
        auto out_tensor_strides = p_out_coords_key->getTensorStride();
        Region region =
            Region(out_tensor_strides, kernel_sizes, dilations, region_type,
                   offsets.data<int>(), offsets.size(0));

        // Flip in and out
        auto out_in = out_map.kernel_map(in_map, region);

        in_maps[map_key] = move(out_in.second);
        out_maps[map_key] = move(out_in.first);
      }
    }
    return make_pair(ref(in_maps[map_key]), ref(out_maps[map_key]));
  }
}

template <typename MapType>
const InOutMapsRefPair<int>
CoordsManager<MapType>::getOriginInOutMaps(py::object py_in_coords_key,
                                           py::object py_out_coords_key) {
  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();

  const int D = p_in_coords_key->getDimension();
  // Create output coordinates if it doesn't exist
  if (!p_out_coords_key->key_set) {
    p_out_coords_key->setKey(createOriginCoords(D));
    const vector<int> zero_vec(D, 0);
    p_out_coords_key->setTensorStride(zero_vec);
  } else {
    ASSERT(coords_maps.find(p_out_coords_key->getKey()) != coords_maps.end(),
           "Global map not initialized.");
  }

  const uint64_t in_coords_key = p_in_coords_key->getKey();
  const uint64_t out_coords_key = p_out_coords_key->getKey();

  // Map key for origin hash map
  const InOutMapKey map_key =
      getOriginMapHashKey(py_in_coords_key, py_out_coords_key);

  // For non transpose case
  // make a kernel mapping. The kernel will be saved with the map_key.
  if (in_maps.find(map_key) == in_maps.end()) {
    ASSERT(coords_maps[out_coords_key].size() == batch_indices.size(),
           "Coords size mismatch. CoordsMap size: ",
           coords_maps[out_coords_key].size(),
           ", batch size: ", batch_indices.size());
    const auto in_out = coords_maps[in_coords_key].global_reduction_map(
        coords_maps[out_coords_key]);
    in_maps[map_key] = in_out.first;
    out_maps[map_key] = in_out.second;
  }
  return make_pair(ref(in_maps[map_key]), ref(out_maps[map_key]));
}

template <typename MapType>
const InOutMapsRefPair<int>
CoordsManager<MapType>::getPruningInOutMaps(at::Tensor use_feat,
                                            py::object py_in_coords_key,
                                            py::object py_out_coords_key) {
  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();

  // Create output coordinates if it doesn't exist
  if (!p_out_coords_key->key_set) {
    // The following function setup py_out_coords_key
    createPrunedCoords(use_feat, py_in_coords_key, py_out_coords_key);
  }

  const uint64_t in_coords_key = p_in_coords_key->getKey();
  const uint64_t out_coords_key = p_out_coords_key->getKey();

  // Use the map key for origin hash map (stride, dilation, kernel are all
  // NULL)
  const InOutMapKey map_key =
      getOriginMapHashKey(py_in_coords_key, py_out_coords_key);

  // For non transpose case
  // make a kernel mapping. The kernel will be saved with the map_key.
  if (in_maps.find(map_key) == in_maps.end()) {
    const auto in_out = coords_maps[in_coords_key].pruned_kernel_map(
        coords_maps[out_coords_key]);
    in_maps[map_key] = in_out.first;
    out_maps[map_key] = in_out.second;
  }

  return make_pair(ref(in_maps[map_key]), ref(out_maps[map_key]));
}

template <typename MapType>
const InOutMapsRefPair<int>
CoordsManager<MapType>::getUnionInOutMaps(vector<py::object> py_in_coords_keys,
                                          py::object py_out_coords_key) {
  CoordsKey *p_out_coords_key = py_out_coords_key.cast<CoordsKey *>();

  // Create output coordinates if it doesn't exist
  if (!p_out_coords_key->key_set)
    createUnionCoords(py_in_coords_keys, py_out_coords_key);

  const uint64_t out_coords_key = p_out_coords_key->getKey();

  // Map key for origin hash map
  const InOutMapKey map_key =
      getUnionMapHashKey(py_in_coords_keys, py_out_coords_key);

  vector<reference_wrapper<CoordsMap<MapType>>> in_coords_maps;
  for (const auto &py_in_coords_key : py_in_coords_keys) {
    const CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
    uint64_t in_coords_key = p_in_coords_key->getKey();
    in_coords_maps.push_back(ref(coords_maps[in_coords_key]));
  }

  // For non transpose case
  // make a kernel mapping. The kernel will be saved with the map_key.
  if (in_maps.find(map_key) == in_maps.end()) {
    const auto in_out = CoordsMap<MapType>::union_map(
        in_coords_maps, coords_maps[out_coords_key]);
    in_maps[map_key] = in_out.first;
    out_maps[map_key] = in_out.second;
  }

  return make_pair(ref(in_maps[map_key]), ref(out_maps[map_key]));
}

template <typename MapType> string CoordsManager<MapType>::toString() const {
  Formatter out;
  out << "< CoordsManager\n\tNumber of Coordinate Maps: "
      << to_string(coords_maps.size());
  for (const auto &kv : coords_maps) {
    out << " \n\t\tCoordinate Map Key: " << to_string(kv.first)
        << ", Size: " << to_string((kv.second).size());
  }
  out << "\n\tNumber of Kernel Maps: " << to_string(in_maps.size());
  for (const auto &kv : in_maps) {
    size_t size = 0;
    for (const auto &map : kv.second)
      size += map.size();
    out << " \n\t\tKernel In-Out Map Key: "
        << to_string(hash_vec<InOutMapKey>(kv.first))
        << ", Size: " << to_string(size);
  }
  out << " >\n";
  return out;
}

*/
/*
 * Return row indices for each batch index
 */
/*
template <typename MapType>
at::Tensor
CoordsManager<MapType>::getRowIndicesAtBatchIndex(py::object py_in_coords_key,
                                                  py::object py_out_coords_key,
                                                  const int batch_index) {
  // py_out_coords_key will be set after the above call.
  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  const auto in_coords_key = p_in_coords_key->getKey();
  ASSERT(coords_maps.find(in_coords_key) != coords_maps.end(),
         "The in_coords_key, ", to_string(in_coords_key), ", does not exist.");
  // Find the batch index in the current batch indices.
  const vector<int>::iterator batch_iter = std::find(
      vec_batch_indices.begin(), vec_batch_indices.end(), batch_index);

  // ASSERT(batch_iter != vec_batch_indices.end(),
  //        "Invalid batch index:", batch_index,
  //        " does not exist in the provided batch indices:",
  //        ArrToString(vec_batch_indices));

  // Return an empty list if not found.
  if (batch_iter == vec_batch_indices.end()) {
    at::Tensor in_rows =
        torch::zeros({0}, torch::TensorOptions().dtype(torch::kInt64));
    return in_rows;

  } else {

    const auto in_outs =
        getOriginInOutMaps(py_in_coords_key, py_out_coords_key);
    const auto &in = in_outs.first[*batch_iter];

    at::Tensor in_rows = torch::zeros(
        {(long)in.size()}, torch::TensorOptions().dtype(torch::kInt64));

    // copy all from a vector. int -> long
    auto a_in_rows = in_rows.accessor<long, 1>();
    for (auto i = 0; i < in.size(); i++)
      a_in_rows[i] = in[i];

    return in_rows;
  }
}
*/
/*
 * Return row indices per batch
 */
/*
template <typename MapType>
vector<at::Tensor>
CoordsManager<MapType>::getRowIndicesPerBatch(py::object py_in_coords_key,
                                              py::object py_out_coords_key) {
  // py_out_coords_key will be set after the above call.
  CoordsKey *p_in_coords_key = py_in_coords_key.cast<CoordsKey *>();
  const auto in_coords_key = p_in_coords_key->getKey();
  ASSERT(coords_maps.find(in_coords_key) != coords_maps.end(),
         "The in_coords_key, ", to_string(in_coords_key), ", does not exist.");

  const auto in_outs = getOriginInOutMaps(py_in_coords_key, py_out_coords_key);
  const auto &ins = in_outs.first;

  // Return index.
  vector<at::Tensor> out_inds;
  for (const auto &in : ins) {
    at::Tensor mapping = torch::zeros(
        {(long)in.size()}, torch::TensorOptions().dtype(torch::kInt64));

    // copy all from a vector, int -> long
    auto a_mapping = mapping.accessor<long, 1>();
    for (auto i = 0; i < in.size(); i++)
      a_mapping[i] = in[i];

    // ::memcpy(mapping.data<int>(), in.data(), in.size() * sizeof(int));
    out_inds.push_back(mapping);
  }

  return out_inds;
}
*/

/* Helper functions */
template <typename coordinate_type,
          template <typename C> class TemplatedAllocator,
          template <typename T, template <typename Q> class A>
          class CoordinateMapType>
at::Tensor
CoordinateMapManager<coordinate_type, TemplatedAllocator, CoordinateMapType>::
    get_coordinates(CoordinateMapKey const *p_key) const {
  ASSERT(exists(p_key), ERROR_MAP_NOT_FOUND);
  auto const it = m_coordinate_maps.find(p_key->get_key());
  ASSERT(it != m_coordinate_maps.end(), ERROR_MAP_NOT_FOUND);
  auto const &map = it->second;
  auto const nrows = map.size();
  auto const ncols = map.coordinate_size();

  // CPU torch.IntTensor
  auto options = torch::TensorOptions().dtype(torch::kInt).requires_grad(false);
  if (!detail::is_cpu_coordinate_map<CoordinateMapType>::value) {
#ifndef CPU_ONLY
    int device_id;
    CUDA_CHECK(cudaGetDevice(&device_id));
    options = options.device(torch::kCUDA, device_id);
#else
    ASSERT(false, ERROR_CPU_ONLY);
#endif
  }
  at::Tensor coordinates = torch::empty({(long)nrows, (long)ncols}, options);

  // copy to the out coords
  map.copy_coordinates(coordinates.template data_ptr<coordinate_type>());
  return coordinates;
}

template class CoordinateMapManager<default_types::dcoordinate_type,
                                    std::allocator, CoordinateMapCPU>;

} // end namespace minkowski