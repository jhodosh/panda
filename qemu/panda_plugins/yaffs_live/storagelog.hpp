#ifndef __STORAGELOG_HPP
#define __STORAGELOG_HPP

#include <fstream>
#include "storagelog.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <map>

enum class MetadataType {
    NO_METADATA,
    YAFFSHEADER,
    YAFFSSPARE,
};
struct Metadata{
    MetadataType type;
    union {
        std::nullptr_t nothing;
        YaffsSpare* yaffsspare;
        YaffsHeader* yaffsheader;
    };
};

typedef std::map<uint64_t, uint64_t> LogIndex;

class Logfile {
protected:
    std::fstream logfile;
    std::fstream indexfile;
    uint64_t logindex = 0;
    LogIndex index;
public:
    bool ready(){
        return logfile.is_open() && indexfile.is_open();
    }
    
};

class LogReader : public Logfile {
public:
    LogReader(std::string filename);
};

class LogWriter: public Logfile {
public:
    LogWriter(std::string filename);
    bool readSector(uint64_t address, uint8_t* data,
                    uint32_t length, Metadata metadata);
    bool wroteSector(uint64_t address, uint8_t* data,
                     uint32_t length, Metadata metadata);
    bool erasedSector(uint64_t address, uint32_t length);
};


LogWriter::LogWriter(std::string filename)
{
    logfile.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    indexfile.open(filename.append(".idx"), std::ios::trunc | std::ios::out | std::ios::binary);
    
}

LogReader::LogReader(std::string filename)
{
    logfile.open(filename, std::ios::in |  std::ios::binary);
    indexfile.open(filename.append(".idx"), std::ios::in | std::ios::binary);
    if (ready()){
        // Read in the index
        LogIndex::key_type id;
        LogIndex::mapped_type offset;
        while(indexfile.good()){
            indexfile.read(reinterpret_cast<char*>(&id), sizeof(id));
            if(!indexfile.good()) break;
            indexfile.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            if(!indexfile.good()) break;
            index[id] = offset;
        }
    }
}

static bool writeDelimitedTo(google::protobuf::Message& message, std::fstream& out)
{
    { // The google classes write on dstor
        google::protobuf::io::OstreamOutputStream _outstream(&out);
        google::protobuf::io::CodedOutputStream coded_output(&_outstream);

        coded_output.WriteLittleEndian32(message.ByteSize());
        message.SerializeToCodedStream(&coded_output);
    } // dstor google objects so we can check if the write succeeded
    return out.good();
}

static bool readDelimitedFrom(google::protobuf::Message& message, std::fstream& in){
    google::protobuf::uint32 size;
    google::protobuf::io::IstreamInputStream _instream(&in);
    google::protobuf::io::CodedInputStream coded_input(&_instream);
    bool has_next = coded_input.ReadLittleEndian32(&size);
    if(!has_next) {
        return false;
    } else {
        google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(size);
        if ( message.ParseFromCodedStream(&coded_input) ) {
            coded_input.PopLimit(msgLimit);
            return true;
        }
        return false;
    }
}

bool LogWriter::readSector(uint64_t address, uint8_t* data,
                         uint32_t length, Metadata metadata)
{
    StorageLog::LogEntry entry;
    entry.set_address(address);
    entry.set_op(StorageLog::LogEntry_Operation_READ);
    entry.set_logindex(logindex);
    entry.set_data(data, length);
    if(metadata.type == MetadataType::YAFFSHEADER){
        entry.set_mtype(StorageLog::LogEntry_MetadataType_YAFFSHEADER);
        entry.set_metadata(metadata.yaffsheader, sizeof(metadata.yaffsheader));
    } else if(metadata.type == MetadataType::YAFFSSPARE){
        entry.set_mtype(StorageLog::LogEntry_MetadataType_YAFFSSPARE);
        entry.set_metadata(metadata.yaffsspare, sizeof(metadata.yaffsspare));
    }
    auto offset = logfile.tellp();
    index[logindex] = offset;
    
    if(!writeDelimitedTo(entry, logfile))
        return false;
    indexfile.write(reinterpret_cast<char*>(&logindex), sizeof(logindex));
    indexfile.write(reinterpret_cast<char*>(&offset), sizeof(offset));
    logindex++;
    return true;
}


bool LogWriter::wroteSector(uint64_t address, uint8_t* data,
                          uint32_t length, Metadata metadata)
{
    StorageLog::LogEntry entry;
    entry.set_address(address);
    entry.set_op(StorageLog::LogEntry_Operation_WRITE);
    entry.set_logindex(logindex);
    entry.set_data(data, length);
    if(metadata.type == MetadataType::YAFFSHEADER){
        entry.set_mtype(StorageLog::LogEntry_MetadataType_YAFFSHEADER);
        entry.set_metadata(metadata.yaffsheader, sizeof(metadata.yaffsheader));
    } else if(metadata.type == MetadataType::YAFFSSPARE){
        entry.set_mtype(StorageLog::LogEntry_MetadataType_YAFFSSPARE);
        entry.set_metadata(metadata.yaffsspare, sizeof(metadata.yaffsspare));
    }
    auto offset = logfile.tellp();
    index[logindex] = offset;
    if(!writeDelimitedTo(entry, logfile))
        return false;
    indexfile.write(reinterpret_cast<char*>(&logindex), sizeof(logindex));
    indexfile.write(reinterpret_cast<char*>(&offset), sizeof(offset));
    logindex++;
    return true;
}

bool LogWriter::erasedSector(uint64_t address, uint32_t length)
{
    StorageLog::LogEntry entry;
    entry.set_address(address);
    entry.set_op(StorageLog::LogEntry_Operation_ERASE);
    entry.set_logindex(logindex);
    entry.set_length(length);
     auto offset = logfile.tellp();
    index[logindex] = offset;
    if(!writeDelimitedTo(entry, logfile))
        return false;
    indexfile.write(reinterpret_cast<char*>(&logindex), sizeof(logindex));
    indexfile.write(reinterpret_cast<char*>(&offset), sizeof(offset));
    logindex++;
    return true;
}


#endif
