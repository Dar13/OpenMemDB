#include <atomic>
#include <limits>

#include "tervel/util/memory/hp/hp_element.h"
#include "tervel/containers/wf/vector/vector.hpp"

using namespace tervel::util::memory::hp;

template <typename S>
class ValuePointer : public tervel::util::memory::hp::Element
{
    public:
        S* value;
        std::atomic<int64_t> counter;

        bool on_is_watched() {
            return (counter.load() > 0);
        }

        ~ValuePointer() {
            counter.store(std::numeric_limits<int64_t>::min());
        }
};

template <typename T>
class VectorAccessor {
    public:
        using Value = ValuePointer<T>;

        Value* value;

        bool init(tervel::containers::wf::vector::Vector<Value*>& vector, uint32_t index) {
            Value* retrieved_value;
            if(vector.at(index, retrieved_value))
            {
                // hp_watch
                if(HazardPointer::watch(HazardPointer::SlotID::SHORTUSE, retrieved_value))
                {
                    // TODO: ?
                }

                // c != vector->at(x))
                // TODO: What is c?

                uintptr_t clean_ptr = reinterpret_cast<uintptr_t>(retrieved_value) & (~0xF);
                retrieved_value = reinterpret_cast<Value*>(clean_ptr);

                int64_t res = retrieved_value->counter.fetch_add(1);

                // hp_unwatch
                HazardPointer::unwatch(HazardPointer::SlotID::SHORTUSE, retrieved_value);

                T* check_2 = nullptr;
                // TODO: Handle a failure here?
                vector.at(index, check_2);
                check_2 = (Value*)(((uintptr_t)check_2) & (~0xF));
                if(res < 0 || retrieved_value != check_2)
                {
                    retrieved_value->counter.fetch_sub(1);
                    return false;
                }

                this->value = retrieved_value;
                return true;
            }
            else
            {
                return false;
            }
        }

        ~VectorAccessor() {
            value->counter.fetch_sub(1);
        }
};
