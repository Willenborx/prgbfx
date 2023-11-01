#ifndef EffectArrayAbstract_hpp
#define EffectArrayAbstract_hpp

#include <Effect.hpp>
#include <functional>


namespace prgbfx {
    using namespace prgb;

    /**
     * @brief This template can be used for effects that use a std::vector to manage the shown items. It provides functions to add and remove items
     * and an interface for a lambda to render each item. It takes away the responsibility of cleaning up and because it forbids using pointers
     * it's very unlikely to create memory leaks using this base class
     * 
     * @tparam T Type for the vector elements
     */
    template <typename T>
    class EffectArrayAbstract : public Effect {
        static_assert(!std::is_pointer_v<T>,"Objects belonging to pointers wouldn't be cleaned up. Do not use pointers with this class");

        public:
            EffectArrayAbstract(LightArray& ar) : Effect(ar) { LOG(" EffectArrayAbstract: Construct"); items.reserve(200);} // #desperate
            virtual ~EffectArrayAbstract() { LOG(" EffectArrayAbstract: Destruct");}

        protected:
            /// This vector holds all items that have to be displayed
            std::vector<T> items = std::vector<T>();

            inline void additem(T item) { items.push_back(item);}

            /// @brief  Removes an item (during an interation)
            /// @param it THe iterator
            /// @return the iterator of the next element
            typename std::vector<T>::iterator removeitem(typename std::vector<T>::iterator it) { return items.erase(it);  }
            
            /**
             * @brief This can be used along with a lambda to calculate each individual item in the array
             * The (lambda) function in the implementation must be of type "bool". It returns "false" if the item is not used anymore 
             * (left screen area, lifetime expired, ...), it returns "true" if the item should stay in the list
             */
            void for_each(const std::function<bool(T item)>func) {
                for (typename std::vector<T>::iterator it=items.begin(); it != items.end();) {
                    if (!func(*it)) { 
                        it = items.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

    };
};
#endif