#ifndef DATA_ACCESSOR_H
#define DATA_ACCESSOR_H

// STL includes
#include <atomic>
#include <limits>

// Tervel includes
#include "tervel/util/memory/hp/hp_element.h"
#include "tervel/containers/wf/vector/vector.hpp"

// Gets rid of long namespaces
using namespace tervel::util::memory::hp;

template <typename S>
class ValuePointer : public tervel::util::memory::hp::Element
{
    public:
        S* ptr;
        std::atomic<int64_t> counter;

        ValuePointer(S* value) : ptr(value), counter(0) {}

        bool on_is_watched() {
            return (counter.load() > 0);
        }

        ~ValuePointer() {
            counter.store(std::numeric_limits<int64_t>::min());
            delete ptr;
        }
};

template <typename T>
class VectorAccessor {
    public:
        using Value = ValuePointer<T>;

        Value* value;

        VectorAccessor() : value(nullptr) {}

        bool init(tervel::containers::wf::vector::Vector<Value*>& vector, uint32_t index) {
            ///////////////////////
            // MAGIC DON'T TOUCH //
            ///////////////////////
            Value* retrieved_value;
            Value* value_ptr;
            if(vector.at(index, retrieved_value))
            {
                // hp_watch
                HazardPointer::watch(HazardPointer::SlotID::SHORTUSE, retrieved_value);

                Value* test_value;
                if(!vector.at(index, test_value) || retrieved_value != test_value)
                {
                    HazardPointer::unwatch(HazardPointer::SlotID::SHORTUSE, retrieved_value);
                    return false;
                }

                uintptr_t clean_ptr = reinterpret_cast<uintptr_t>(retrieved_value) & (~0x7);
                value_ptr = reinterpret_cast<Value*>(clean_ptr);

                int64_t res = value_ptr->counter.fetch_add(1);

                // hp_unwatch
                HazardPointer::unwatch(HazardPointer::SlotID::SHORTUSE, retrieved_value);

                Value* check_2 = nullptr;
                if(!vector.at(index, check_2))
                {
                    HazardPointer::unwatch(HazardPointer::SlotID::SHORTUSE, retrieved_value);
                    return false;
                }

                if(res < 0 || retrieved_value != check_2)
                {
                    retrieved_value->counter.fetch_sub(1);
                    return false;
                }

                // Store the clean pointer and return true, the access is successful.
                this->value = value_ptr;
                return true;
            }
            else
            {
                return false;
            }
        }

        ~VectorAccessor() {
            if(value)
            {
                value->counter.fetch_sub(1);
            }
        }
};

#endif
