#include <array>
#include <string>
#include <vector>

template <typename T>
inline T &readElement(T *data, const std::vector<size_t> &dims,
                      const std::vector<size_t> &indices) {
  size_t elem_offset = 0;
  size_t dim_offset = 1;
  size_t ndims = dims.size();
  for (int i = ndims - 1; i >= 0; --i) {
    elem_offset += indices[i] * dim_offset;
    dim_offset *= dims[i];
  }
  return data[elem_offset];
}

std::array<size_t, 256> getIndexTable(const std::string_view subexpr) {
  std::array<size_t, 256> table;
  std::fill(table.begin(), table.end(), -1);

  size_t n = subexpr.size();
  for (int i = 0; i < n; ++i) {
    table[subexpr[i]] = i;
  }
  return table;
}

template <typename T>
void einum(const std::string &expr, const T *a_ptr,
           const std::vector<size_t> &a_dims, const T *b_ptr,
           const std::vector<size_t> &b_dims, T *c_ptr,
           std::vector<size_t> &c_dims) {

  // Translate expression
  size_t split_0 = expr.find(',');
  size_t split_1 = expr.find("->");

  const std::string_view a_expr = expr.substr(0, split_0);
  const std::string_view b_expr =
      expr.substr(split_0 + 1, split_1 - split_0 - 1);
  const std::string_view c_expr = expr.substr(split_1 + 2);

  const auto a_index_table = getIndexTable(a_expr);
  const auto b_index_table = getIndexTable(b_expr);
  const auto c_index_table = getIndexTable(c_expr);

  std::string a_reduced_dims, a_broadcasted_dims;
  std::string b_reduced_dims, b_broadcasted_dims;
  std::string contracted_dims;
  for (int i = 0; i < 256; ++i) {
    bool inside_a = a_index_table[i] != -1;
    bool inside_b = b_index_table[i] != -1;
    bool inside_c = c_index_table[i] != -1;
    char ch = static_cast<char>(i);
    if (!inside_a && !inside_b) {
      assert(!inside_c);
      continue;
    } else if (inside_a && !inside_b) {
      if (inside_c) {
        a_broadcasted_dims += ch;
      } else {
        a_reduced_dims += ch;
      }
    } else if (!inside_a && inside_b) {
      if (inside_c) {
        b_broadcasted_dims += ch;
      } else {
        b_reduced_dims += ch;
      }
    } else {
      // inside_a && inside_b
      if (inside_c) {
        a_broadcasted_dims += ch;
        b_broadcasted_dims += ch;
      } else {
        contracted_dims += ch;
      }
    }
  }

  // Figure out c dimension
  std::array<size_t, 256> dim_size_table;
  for (int i = 0; i < 256; ++i) {
    dim_size_table[i] = a_index_table[i] >= 0   ? a_dims[a_index_table[i]]
                        : b_index_table[i] >= 0 ? b_dims[b_index_table[i]]
                                                : -1;
  }
  const size_t n_c_dims = c_expr.size();
  c_dims.resize(n_c_dims);
  for (int i = 0; i < c_expr.size(); ++i) {
    c_dims[i] = dim_size_table[c_expr[i]];
  }

  vector<size_t> a_index(a_dims.size(), 0);
  vector<size_t> b_index(b_dims.size(), 0);
  vector<size_t> c_index(n_c_dims, 0);

  function<void(int, int, int)> contract_ab;
  contract_ab = [&](int dim) {
    T sum = 0;
    if (dim < a_reduced_dims.size()) {
      size_t a_dim = a_index_table[a_reduced_dims[dim]];
      for (int i = 0; i < a_dims[a_dim]; ++i) {
        a_index[a_dim] = i;
        sum += contract_ab(dim + 1);
      }
    } else if (dim < a_reduced_dims.size() + b_reduced_dims.size()) {
      dim -= a_reduced_dims.size() size_t b_dim =
          b_index_table[b_reduced_dims[dim]];
      for (int i = 0; i < b_dims[b_dim]; ++i) {
        b_index[b_dim] = i;
        sum += contract_ab(dim + 1);
      }
    } else if (dim < a_reduced_dims.size() + b_reduced_dims.size() +
                         contracted_dims.size()) {
      dim -= a_reduced_dims.size() + b_reduced_dims.size();
      size_t a_dim = a_index_table[contracted_dims[dim]];
      size_t b_dim = b_index_table[contracted_dims[dim]];
      for (int i = 0; i < a_dims[a_dim]; ++i) {
        a_index[a_dim] = i;
        b_index[b_dim] = i;
        sum += contract_ab(dim + 1);
      }
    } else {
      return readElement(a_ptr, a_dims, a_index) *
             readElement(b_ptr, b_dims, b_index);
    }
    return sum;
  };

  function<void(int)> calc_c;
  calc_c = [&](int dim) {
    if (dim == n_c_dims) {
      T &c_elem = readElement(c_ptr, c_dims, c_index);
      c_elem = contract_ab(0);
      return;
    }
    char dim_ch = c_expr[dim];
    for (int i = 0; i < c_dims[dim]; ++i) {
      c_index[dim] = i;
      if (a_broadcasted_dims.find(dim_ch) != std::string::npos) {
        a_index[a_index_table[dim_ch]] = i;
      }
      if (b_broadcasted_dims.find(dim_ch) != std::string::npos) {
        b_index[b_index_table[dim_ch]] = i;
      }
      calc(dim + 1);
    }
  };

  calc_c(0);
}
