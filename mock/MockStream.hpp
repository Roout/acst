#ifndef MOCK_STREAM_HPP__
#define MOCK_STREAM_HPP__

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <array>
#include <cstddef>

/**
 * Stream concept requirements:
 * https://www.boost.org/doc/libs/1_75_0/libs/beast/doc/html/beast/using_io/stream_types.html
*/
struct MockAsyncStream {

    using executor_type = boost::asio::io_context::executor_type;

    boost::asio::io_context::executor_type m_executor;

    std::byte *m_buffer { nullptr };
    size_t m_size { 0 };
    
    MockAsyncStream(boost::asio::io_context::executor_type& ex);

    boost::asio::io_context::executor_type get_executor() noexcept {
        return m_executor;
    }

    template<class ConstBufferSequence, class WriteHandler>
    void async_write_some(const ConstBufferSequence & buffers, WriteHandler && handler);

    template<class MutableBufferSequence, class ReadHandler>
    void async_read_some(MutableBufferSequence & buffers, ReadHandler && handler);
};

/**
 *  == IMPLEMENTATION ==
 */  

template<class ConstBufferSequence, class WriteHandler>
void MockAsyncStream::async_write_some(
    const ConstBufferSequence & buffers, 
    WriteHandler && handler
) {
    boost::system::error_code error;
    size_t bytes { 0 };
    if (boost::asio::buffer_size(buffers) > 0) {
        // initiates an asynchronous operation to write one or more bytes of data 
        // to the stream a from the buffer sequence cb. 
        bytes = boost::asio::buffer_copy(boost::asio::buffer(m_buffer, m_size), buffers);
        m_buffer += bytes; m_size -= bytes;
        if (!bytes) {
            error = make_error_code(boost::asio::stream_errc::eof);
        }
    }
    boost::asio::post(boost::asio::bind_executor(
        m_executor, 
        std::bind(std::forward<WriteHandler>(handler), error, bytes))
    );
}

/**
 * @param buffer must have size >= m_buffer.size()
 */
template<class MutableBufferSequence, class ReadHandler>
void MockAsyncStream::async_read_some(
    MutableBufferSequence & buffer, 
    ReadHandler && handler
) {
    boost::system::error_code error;
    size_t bytes { 0 };
    if (m_size > 0) {
        bytes = boost::asio::buffer_copy(buffer, boost::asio::buffer(m_buffer, m_size));
        m_buffer += bytes; m_size -= bytes;
        if (!bytes) {
            error = make_error_code(boost::asio::stream_errc::eof);
        }
    }
    boost::asio::post(boost::asio::bind_executor(
        m_executor, 
        std::bind(std::forward<ReadHandler>(handler), error, bytes))
    );  

}

#endif // MOCK_STREAM_HPP__