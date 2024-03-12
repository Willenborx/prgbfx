/**
 * @file EffectArrayAbstract.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief 
 * @version 0.5
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef EffectArrayAbstract_hpp
#define EffectArrayAbstract_hpp

#include <Effect.hpp>
#include <functional>

namespace prgbfx {
    using namespace prgb;

    /**
     * @brief This template can be used for effects that use a std::list to manage the shown items. It provides functions to add and remove items
     * and an interface for a lambda to render each item. It takes away the responsibility of cleaning up and because it forbids using pointers
     * it's very unlikely to create memory leaks using this base class
     * 
     * @tparam T Type for the list elements
     */
    template <typename T>
    class EffectArrayAbstract : public Effect {
        static_assert(!std::is_pointer_v<T>,"Objects belonging to pointers wouldn't be cleaned up. Do not use pointers with this class");

        public:
            EffectArrayAbstract(LightArray* ar) : Effect(ar) { LOG(" EffectArrayAbstract: Construct"); } 
            virtual ~EffectArrayAbstract() { LOG(" EffectArrayAbstract: Destruct");}

        protected:
            /// This list holds all items that have to be displayed
            std::list<T> items = std::list<T>();

            inline void additem(const T item) { items.push_back(item);}

            /// @brief  Removes an item (during an interation)
            /// @param it THe iterator
            /// @return the iterator of the next element
            typename std::list<T>::iterator removeitem(typename std::list<T>::iterator it) { return items.erase(it);  }
            
            /**
             * @brief This can be used along with a lambda to calculate each individual item in the array
             * The (lambda) function in the implementation must be of type "bool". It returns "false" if the item is not used anymore 
             * (left screen area, lifetime expired, ...), it returns "true" if the item should stay in the list
             */
            void for_each(std::function<bool(T& item)>func) {
                for (typename std::list<T>::iterator it=items.begin(); it != items.end();) {
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