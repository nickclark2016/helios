#pragma once

#include <helios/macros.hpp>
#include <helios/containers/utility.hpp>

#include <assert.h>

namespace helios
{
    /// <summary>
    /// Unified result type containing a pointer to a result or an error code.  The result does not own the pointer it is constructed from.
    /// </summary>
    /// <typeparam name="ResultType">Type of the error.</typeparam>
    template <typename ResultType>
    class Result
    {
        Result() = default;

    public:
        ~Result() = default;

        /// <summary>
        /// Accessor for fields and methods of the result, asserting that it is a successful result.
        /// </summary>
        /// <returns>Pointer to the result.</returns>
        ResultType* operator->() noexcept
        {
            return get();
        }

        /// <summary>
        /// Accessor for fields and methods of the result, asserting that it is a successful result.
        /// </summary>
        /// <returns>Pointer to the result.</returns>
        const ResultType* operator->() const noexcept
        {
            return get();
        }

        /// <summary>
        /// Accessor for fields and methods of the result, asserting that it is a successful result.
        /// </summary>
        /// <returns>Reference to the result.</returns>
        ResultType& operator*() noexcept
        {
            return *get();
        }

        /// <summary>
        /// Accessor for fields and methods of the result, asserting that it is a successful result.
        /// </summary>
        /// <returns>Reference to the result.</returns>
        const ResultType& operator*() const noexcept
        {
            return *get();
        }

        /// <summary>
        /// Gets a pointer to the result, asserting that it is a successful result.
        /// </summary>
        /// <returns>Pointer to the result.</returns>
        ResultType* get() noexcept
        {
            assert(_isSuccess);
            return _value;
        }

        /// <summary>
        /// Gets a pointer to the result, asserting that it is a successful result.
        /// </summary>
        /// <returns>Pointer to the result.</returns>
        const ResultType* get() const noexcept
        {
            assert(_isSuccess);
            return _value;
        }

        /// <summary>
        /// Checks if the result was successful or contains an error.
        /// </summary>
        /// <returns>True if the result is successful, else false.</returns>
        explicit operator bool() const noexcept
        {
            return _isSuccess;
        }

        /// <summary>
        /// Gets the error code of the result, asserting that it is not a successful result.
        /// </summary>
        /// <returns>User defined error code.</returns>
        u64 error() const noexcept
        {
            assert(!_isSuccess);
            return _errorCode;
        }

        /// <summary>
        /// Creates a successful result type.
        /// </summary>
        /// <param name="value">Pointer to value to store in result. The ownership of the pointer remains with the caller.</param>
        /// <returns>Successful result containing the pointer to the value</returns>
        static Result<ResultType> fromSuccess(ResultType* value)
        {
            Result<ResultType> res;
            res._isSuccess = true;
            res._value = value;
            return res;
        }

        /// <summary>
        /// Creates an error result type.
        /// </summary>
        /// <param name="errorCode">User defined error code to store in result.</param>
        /// <returns>Error result containing the error code.</returns>
        static Result<ResultType> fromError(const u64 errorCode)
        {
            Result<ResultType> res;
            res._isSuccess = false;
            res._errorCode = errorCode;
            return res;
        }

        /// <summary>
        /// Used to create a result type who's success condition is based on a success flag.
        /// </summary>
        /// <param name="isSuccessful">Flag defining if the result is a successful type.</param>
        /// <param name="value">Pointer to value to store in result. The ownership of the pointer remains with the caller.  This is only used if isSuccessful is true.</param>
        /// <param name="errorCode">User defined error code to store in result.  This is only used if isSuccessful is false.</param>
        /// <returns>Result containing either the value or the error code depending on the isSuccessful flag.</returns>
        static Result<ResultType> fromConditional(const bool isSuccessful, ResultType* value, const u64 errorCode)
        {
            Result<ResultType> res;
            res._isSuccess = isSuccessful;

            if (isSuccessful)
            {
                res._value = value;
            }
            else
            {
                res._errorCode = errorCode;
            }
            
            return res;
        }

    private:
        bool _isSuccess;
        union
        {
            ResultType* _value;
            u64 _errorCode;
        };
    };
} // namespace helios
