#include "stdafx.h"
#include "common.h"

MsgPacket::MsgPacket() : raw_length(0), raw_data(NULL), locked(0)
{
}
MsgPacket::~MsgPacket()
{
	assert(!locked);
	FreeBuffer();
}
void MsgPacket::GetRawData(const char **pbuf, size_t *len)
{
	assert(!locked);
	*pbuf = raw_data;
	*len = raw_length;
}

void MsgPacket::FreeBuffer()
{
	assert(!locked);
	if (raw_data) {
		free(raw_data);
		raw_length = 0;
		raw_data = NULL;
	}
}
void MsgPacket::AllocBuffer(size_t len, int type)
{
	assert(!locked);
	raw_length = len + sizeof(raw_header);
	raw_data = (char *) realloc(raw_data, raw_length);
	raw_header hdr = { len, type };
	*(raw_header *)raw_data = hdr;
}
char *MsgPacket::LockBuffer()
{
	assert(!locked);
	locked = 1;
	return raw_data + sizeof(raw_header);
}
void MsgPacket::UnlockBuffer()
{
	assert(locked);
	locked = 0;
}
void MsgPacket::TruncateBuffer(size_t new_size)
{
	size_t new_raw_length = new_size + sizeof(raw_header);
	assert(new_raw_length < raw_length);
	raw_length = new_raw_length;
}

int MsgPacket::LoadHeader(const raw_header *phdr)
{
	if (phdr->data_length > MAX_MSGPACKETSIZE_HARD) return -1;
	AllocBuffer(phdr->data_length, phdr->type);
	return 0;
}

size_t MsgPacket::GetBufferSize()
{
	assert(raw_length >= sizeof(raw_header));
	return raw_length - sizeof(raw_header);
}