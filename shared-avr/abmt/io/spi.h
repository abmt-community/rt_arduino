/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */
#ifndef SHARED_ABMT_IO_SPI_H_
#define SHARED_ABMT_IO_SPI_H_

#include <cstdint>
#include <memory>
#include <abmt/blob.h>

namespace abmt{
namespace io{

struct spi{

	/// This is the only function that you need to override to make the others work.
	virtual uint8_t tranceive(uint8_t c){
		return 0;
	}

	virtual void write(uint8_t c){
		tranceive(c);
	}

	virtual uint8_t read(uint8_t data_to_send = 0){
		return tranceive(data_to_send);
	}

	/// writes data to the bus and puts the received data in the blob
	virtual void tranceive(abmt::blob& data){
		for(size_t i = 0; i < data.size; i++){
			data.data[i] = tranceive(data.data[i]);
		}
	}

	virtual void write(abmt::blob& data){
		//auto copy = abmt::blob_shared(data);
		tranceive(data);
	}

	virtual void read(abmt::blob& data, uint8_t data_to_send = 0){
		memset(data.data, data_to_send, data.size);
		tranceive(data);
	}

	virtual ~spi(){

	}
};

using spi_ptr = std::shared_ptr<spi>;

} // namespace io
} // namespace abmt

#endif /* SHARED_ABMT_IO_SPI_H_ */
