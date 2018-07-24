#pragma once

#include <cstddef>
#include <cstdint>
#include <sys/socket.h>
#include <arpa/inet.h>

class Checksum {

private:
	uint32_t sum = 0;
	bool	 odd = false;

public:
	Checksum& add(const void* p, size_t len);
	Checksum& add(const iovec& iov);
	Checksum& add(uint16_t n);
	uint16_t value() const;

};

inline uint16_t Checksum::value() const
{
	uint32_t tmp = sum;
	while (tmp >> 16) {
		tmp = (tmp & 0xffff) + (tmp >> 16);
	}
	tmp = ~tmp;

	return static_cast<uint16_t>(htons(tmp));
}

inline Checksum& Checksum::add(const void* p, size_t n)
{
	auto x = reinterpret_cast<const uint8_t*>(p);

	while (n) {
		auto c = *x++;
		if (odd) {
			sum += c;
		} else {
			sum += (c << 8);
		}
		odd = !odd;
		--n;
	}

	return *this;
}

inline Checksum& Checksum::add(const iovec& iov)
{
	return add(iov.iov_base, iov.iov_len);
}

inline Checksum& Checksum::add(uint16_t n)
{
	n = htons(n);
	add(&n, sizeof n);
	return *this;
}