#pragma once

#include <algorithms/IForest.hpp>
#include <datasets/Instance.hpp>
#include <sketches/BenHaim.hpp>
#include <sketches/DADO.hpp>
#include <sketches/Density.hpp>
#include <string>

template <typename T> struct TypeName {
  using Type = T;
};

template <> struct TypeName<SimpleBenHaim> {
  static std::string get() {
    return "benhaim_simple";
  }
};

template <> struct TypeName<AdvancedBenHaim> {
  static std::string get() {
    return "benhaim_advanced";
  }
};

template <> struct TypeName<IncorrectBenHaim> {
  static std::string get() {
    return "benhaim_incorrect";
  }
};

template <> struct TypeName<AllKDEBenHaim> {
  static std::string get() {
    return "kde_benhaim";
  }
};

template <> struct TypeName<LimitedKDEBenHaim> {
  static std::string get() {
    return "limited_kde_benhaim";
  }
};

template <> struct TypeName<DADO> {
  static std::string get() {
    return "dado";
  }
};

template <> struct TypeName<DADO_Corrected> {
  static std::string get() {
    return "dado_corrected";
  }
};

template <> struct TypeName<IsolationForest<LabeledInstance>> {
  static std::string get() {
    return "iforest";
  }
};