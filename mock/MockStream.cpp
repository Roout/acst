#include "MockStream.hpp"

MockAsyncStream::MockAsyncStream(
    boost::asio::io_context::executor_type& ex
)   
    : m_executor { ex }
{}
