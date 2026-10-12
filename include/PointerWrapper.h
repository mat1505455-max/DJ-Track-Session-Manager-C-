#ifndef POINTERWRAPPER_H
#define POINTERWRAPPER_H

#include <utility>
#include <iostream>
#include <stdexcept> // Added for std::runtime_error

/**
 * PointerWrapper - A template class that wraps a raw pointer
 * 
 * This is Phase 3 of the assignment. Students must analyze and implement
 * a complete pointer wrapper class that manages dynamic memory safely.
 */
template<typename T>
class PointerWrapper {
private:
    T* ptr;  // Raw pointer to the managed object

public:
    // ========== CONSTRUCTION AND DESTRUCTION ==========

    /**
     * Default constructor - creates empty wrapper
     */
    PointerWrapper() : ptr(nullptr) {}

    /**
     * Constructor from raw pointer - wraps the pointer
     */
    explicit PointerWrapper(T* p) : ptr(p) {}

    /**
     * Destructor
     */
    ~PointerWrapper() {
        if(ptr != nullptr){
            delete ptr;
        }
    }

    // ========== COPY OPERATIONS (DELETED) ==========

    PointerWrapper(const PointerWrapper& other) = delete;

    PointerWrapper& operator=(const PointerWrapper& other) = delete;

    // ========== MOVE OPERATIONS ==========

    /**
     * Move constructor
     * FIX: Initialized ptr in the member initialization list to solve -Weffc++ warning
     */
    PointerWrapper(PointerWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    /**
     * Move assignment operator
     */
    PointerWrapper& operator=(PointerWrapper&& other) noexcept {
        if (this != &other){
            delete ptr;
            this->ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // ========== ACCESS OPERATIONS ==========

    /**
     * Dereference operator
     * @throws std::runtime_error if ptr is null
     */
    T& operator*() const {
        if (ptr == nullptr) {
            throw std::runtime_error("Attempted to dereference a null PointerWrapper.");
        }
        return *ptr;
    };

    /**
     * Arrow operator
     * @throws std::runtime_error if ptr is null
     */
    T* operator->() const {
        if (ptr == nullptr) {
            throw std::runtime_error("Attempted to access member via null PointerWrapper.");
        }
        return ptr;
    }

    /**
     * get() function
     * FIX: Removed exception. get() should return nullptr if empty 
     * so that the caller can check validity (e.g. if (wrapper.get()) ).
     */
    T* get() const {
        return ptr;
    }

    // ========== OWNERSHIP MANAGEMENT ==========

    /**
     * release() function
     */
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /**
     * reset() function
     */
    void reset(T* new_ptr = nullptr) {
        if (ptr == new_ptr) {
            return;
        }
        if(ptr != nullptr){
            delete ptr;
        } 
        ptr = new_ptr;
    }

    // ========== UTILITY FUNCTIONS ==========

    /**
     * Boolean conversion operator
     */
    explicit operator bool() const {
        return ptr != nullptr;
    }

    /**
     * Swap two PointerWrapper objects
     */
    void swap(PointerWrapper& other) noexcept {
        std::swap(ptr, other.ptr);
    }
};

// ========== NON-MEMBER FUNCTIONS ==========

/**
 * Helper function to create PointerWrapper
 */
template<typename T, typename... Args>
PointerWrapper<T> make_pointer_wrapper(Args&&... args) {
    return PointerWrapper<T>(new T(std::forward<Args>(args)...));
}

/**
 * Global swap for PointerWrapper
 * FIX: Uses member swap() because 'ptr' is private and cannot be accessed directly here.
 */
template<typename T>
void swap(PointerWrapper<T>& lhs, PointerWrapper<T>& rhs) noexcept {
    lhs.swap(rhs);
}

#endif // POINTERWRAPPER_H