#ifndef _TUE_SG_SCENE_NODE_HPP_INCLUDED_
#define _TUE_SG_SCENE_NODE_HPP_INCLUDED_

#include <tuesday/sg/scene_object.hpp>

namespace tue::sg {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class Fs, class Cs> class basic_scene_node;

///
///
template <class... Fs, class Cs> class basic_scene_node<mp::tseq<Fs...>, Cs> {
  public:
    using object = basic_object<mp::tseq<Fs...>, Cs>;

    template <typename T>
    using object_impl = basic_object_impl<T, mp::tseq<Fs...>, Cs>;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // namespace tue::sg

#endif
