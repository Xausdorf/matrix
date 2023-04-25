#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <utility>

template <class T>
class matrix {
public:
  template <class U>
  struct basic_col_iterator {
    using value_type = T;
    using reference = U&;
    using pointer = U*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    basic_col_iterator() = default;

  private:
    bool check_col_indices(const basic_col_iterator& other) const {
      return _col_index == other._col_index;
    }

  public:
    operator basic_col_iterator<const T>() const {
      return {_data, _col_index, _cols};
    }

    basic_col_iterator& operator++() {
      _data += _cols;
      return *this;
    }

    basic_col_iterator operator++(int) {
      basic_col_iterator temp = *this;
      ++*this;
      return temp;
    }

    basic_col_iterator& operator--() {
      _data -= _cols;
      return *this;
    }

    basic_col_iterator operator--(int) {
      basic_col_iterator temp = *this;
      --*this;
      return temp;
    }

    basic_col_iterator& operator+=(const difference_type& n) {
      _data += static_cast<difference_type>(_cols) * n;
      return *this;
    }

    basic_col_iterator& operator-=(const difference_type& n) {
      _data -= static_cast<difference_type>(_cols) * n;
      return *this;
    }

    friend basic_col_iterator operator+(const basic_col_iterator& left, const difference_type& right) {
      basic_col_iterator result = left;
      return result += right;
    }

    friend basic_col_iterator operator+(const difference_type& left, const basic_col_iterator& right) {
      return right + left;
    }

    friend basic_col_iterator operator-(const basic_col_iterator& left, const difference_type& right) {
      basic_col_iterator result = left;
      return result -= right;
    }

    friend difference_type operator-(const basic_col_iterator& left, const basic_col_iterator& right) {
      return (left._data - right._data) / static_cast<difference_type>(left._cols);
    }

    reference operator[](const difference_type& n) const {
      return *(_data + static_cast<difference_type>(_cols) * n + static_cast<difference_type>(_col_index));
    }

    reference operator*() const {
      return *(_data + _col_index);
    }

    pointer operator->() const {
      return _data + _col_index;
    }

    friend bool operator<(const basic_col_iterator& left, const basic_col_iterator& right) {
      assert(left.check_col_indices(right));
      return left._data < right._data;
    }

    friend bool operator>(const basic_col_iterator& left, const basic_col_iterator& right) {
      assert(left.check_col_indices(right));
      return left._data > right._data;
    }

    friend bool operator<=(const basic_col_iterator& left, const basic_col_iterator& right) {
      assert(left.check_col_indices(right));
      return left._data <= right._data;
    }

    friend bool operator>=(const basic_col_iterator& left, const basic_col_iterator& right) {
      assert(left.check_col_indices(right));
      return left._data >= right._data;
    }

    bool operator==(const basic_col_iterator& other) const {
      assert(check_col_indices(other));
      return _data == other._data;
    }

    bool operator!=(const basic_col_iterator& other) const {
      return !(*this == other);
    }

    basic_col_iterator(pointer data, size_t col_index, size_t cols) : _data(data), _col_index(col_index), _cols(cols) {}

  private:
    pointer _data;
    size_t _col_index;
    size_t _cols;
  };

public:
  using value_type = T;

  using reference = T&;
  using const_reference = const T&;

  using pointer = T*;
  using const_pointer = const T*;

  using iterator = T*;
  using const_iterator = const T*;

  using row_iterator = T*;
  using const_row_iterator = const T*;

  using col_iterator = basic_col_iterator<T>;
  using const_col_iterator = basic_col_iterator<const T>;

public:
  matrix() : _data(nullptr), _rows(0), _cols(0) {}

  matrix(size_t rows, size_t cols) : matrix() {
    if (rows != 0 && cols != 0) {
      _data = new T[rows * cols]();
      _rows = rows;
      _cols = cols;
    }
  }

  template <size_t Rows, size_t Cols>
  matrix(const T (&init)[Rows][Cols]) : _data(new T[Rows * Cols]),
                                        _rows(Rows),
                                        _cols(Cols) {
    for (size_t i = 0; i < rows(); ++i) {
      std::copy_n(init[i], Cols, row_begin(i));
    }
  }

  matrix(const matrix& other) : _rows(other.rows()), _cols(other.cols()) {
    if (other.empty()) {
      _data = nullptr;
    } else {
      _data = new T[other.size()];
      std::copy(other.begin(), other.end(), begin());
    }
  }

  matrix(matrix&& other) : _data(std::exchange(other._data, nullptr)), _rows(other.rows()), _cols(other.cols()) {}

  void swap(matrix& other) {
    std::swap(_data, other._data);
    std::swap(_rows, other._rows);
    std::swap(_cols, other._cols);
  }

  matrix& operator=(matrix&& other) {
    if (&other != this) {
      delete[] _data;
      _data = std::exchange(other._data, nullptr);
      _rows = std::exchange(other._rows, 0);
      _cols = std::exchange(other._cols, 0);
    }
    return *this;
  }

  matrix& operator=(const matrix& other) {
    if (&other != this) {
      matrix(other).swap(*this);
    }
    return *this;
  }

  ~matrix() {
    delete[] _data;
  }

  // Iterators

  iterator begin() {
    return _data;
  }

  iterator end() {
    return begin() + size();
  }

  const_iterator begin() const {
    return _data;
  }

  const_iterator end() const {
    return begin() + size();
  }

  row_iterator row_begin(size_t row_index) {
    assert(row_index < rows());
    return _data + row_index * cols();
  }

  row_iterator row_end(size_t row_index) {
    assert(row_index < rows());
    return row_begin(row_index) + cols();
  }

  const_row_iterator row_begin(size_t row_index) const {
    assert(row_index < rows());
    return _data + row_index * cols();
  }

  const_row_iterator row_end(size_t row_index) const {
    assert(row_index < rows());
    return row_begin(row_index) + cols();
  }

  col_iterator col_begin(size_t col_index) {
    assert(col_index < cols());
    return {begin(), col_index, cols()};
  }

  col_iterator col_end(size_t col_index) {
    assert(col_index < cols());
    return {end(), col_index, cols()};
  }

  const_col_iterator col_begin(size_t col_index) const {
    assert(col_index < cols());
    return {begin(), col_index, cols()};
  }

  const_col_iterator col_end(size_t col_index) const {
    assert(col_index < cols());
    return {end(), col_index, cols()};
  }

  // Size

  size_t rows() const {
    return _rows;
  }

  size_t cols() const {
    return _cols;
  }

  size_t size() const {
    return _rows * _cols;
  }

  bool empty() const {
    return size() == 0;
  }

  // Elements access

  reference operator()(size_t row, size_t col) {
    return _data[row * cols() + col];
  }

  const_reference operator()(size_t row, size_t col) const {
    return _data[row * cols() + col];
  }

  pointer data() {
    return _data;
  }

  const_pointer data() const {
    return _data;
  }

  // Comparison

  friend bool operator==(const matrix& left, const matrix& right) {
    if (left.rows() != right.rows() || left.cols() != right.cols()) {
      return false;
    }
    return std::equal(left.begin(), left.end(), right.begin());
  }

  friend bool operator!=(const matrix& left, const matrix& right) {
    return !(left == right);
  }

  // Arithmetic operations

  matrix& operator+=(const matrix& other) {
    for (size_t i = 0; i < size(); ++i) {
      _data[i] += other.data()[i];
    }
    return *this;
  }

  matrix& operator-=(const matrix& other) {
    for (size_t i = 0; i < size(); ++i) {
      _data[i] -= other.data()[i];
    }
    return *this;
  }

  matrix& operator*=(const_reference factor) {
    for (size_t i = 0; i < size(); ++i) {
      _data[i] *= factor;
    }
    return *this;
  }

  friend matrix operator+(const matrix& left, const matrix& right) {
    matrix result = left;
    return result += right;
  }

  friend matrix operator-(const matrix& left, const matrix& right) {
    matrix result = left;
    return result -= right;
  }

  friend matrix operator*(const matrix& left, const matrix& right) {
    matrix result(left.rows(), right.cols());
    for (size_t i = 0; i < left.rows(); ++i) {
      for (size_t j = 0; j < right.cols(); ++j) {
        result(i, j) = std::inner_product(left.row_begin(i), left.row_end(i), right.col_begin(j), T());
      }
    }
    return result;
  }

  matrix& operator*=(const matrix& other) {
    return *this = (*this * other);
  }

  friend matrix operator*(const matrix& left, const_reference right) {
    matrix result = left;
    return result *= right;
  }

  friend matrix operator*(const_reference left, const matrix& right) {
    return right * left;
  }

private:
  pointer _data;
  size_t _rows;
  size_t _cols;
};