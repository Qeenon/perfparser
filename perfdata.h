#ifndef PERFDATA_H
#define PERFDATA_H

#include <QIODevice>
#include "perfheader.h"
#include "perfattributes.h"
#include "perffeatures.h"

enum PerfEventType {

	/*
	 * If perf_event_attr.sample_id_all is set then all event types will
	 * have the sample_type selected fields related to where/when
	 * (identity) an event took place (TID, TIME, ID, STREAM_ID, CPU,
	 * IDENTIFIER) described in PERF_RECORD_SAMPLE below, it will be stashed
	 * just after the perf_event_header and the fields already present for
	 * the existing fields, i.e. at the end of the payload. That way a newer
	 * perf.data file will be supported by older perf tools, with these new
	 * optional fields being ignored.
	 *
	 * struct sample_id {
	 * 	{ u32			pid, tid; } && PERF_SAMPLE_TID
	 * 	{ u64			time;     } && PERF_SAMPLE_TIME
	 * 	{ u64			id;       } && PERF_SAMPLE_ID
	 * 	{ u64			stream_id;} && PERF_SAMPLE_STREAM_ID
	 * 	{ u32			cpu, res; } && PERF_SAMPLE_CPU
	 *	{ u64			id;	  } && PERF_SAMPLE_IDENTIFIER
	 * } && perf_event_attr::sample_id_all
	 *
	 * Note that PERF_SAMPLE_IDENTIFIER duplicates PERF_SAMPLE_ID.  The
	 * advantage of PERF_SAMPLE_IDENTIFIER is that its position is fixed
	 * relative to header.size.
	 */

	/*
	 * The MMAP events record the PROT_EXEC mappings so that we can
	 * correlate userspace IPs to code. They have the following structure:
	 *
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	u32				pid, tid;
	 *	u64				addr;
	 *	u64				len;
	 *	u64				pgoff;
	 *	char				filename[];
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_MMAP			= 1,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u64				id;
	 *	u64				lost;
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_LOST			= 2,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	u32				pid, tid;
	 *	char				comm[];
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_COMM			= 3,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u32				pid, ppid;
	 *	u32				tid, ptid;
	 *	u64				time;
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_EXIT			= 4,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u64				time;
	 *	u64				id;
	 *	u64				stream_id;
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_THROTTLE			= 5,
	PERF_RECORD_UNTHROTTLE			= 6,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u32				pid, ppid;
	 *	u32				tid, ptid;
	 *	u64				time;
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_FORK			= 7,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u32				pid, tid;
	 *
	 *	struct read_format		values;
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_READ			= 8,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	#
	 *	# Note that PERF_SAMPLE_IDENTIFIER duplicates PERF_SAMPLE_ID.
	 *	# The advantage of PERF_SAMPLE_IDENTIFIER is that its position
	 *	# is fixed relative to header.
	 *	#
	 *
	 *	{ u64			id;	  } && PERF_SAMPLE_IDENTIFIER
	 *	{ u64			ip;	  } && PERF_SAMPLE_IP
	 *	{ u32			pid, tid; } && PERF_SAMPLE_TID
	 *	{ u64			time;     } && PERF_SAMPLE_TIME
	 *	{ u64			addr;     } && PERF_SAMPLE_ADDR
	 *	{ u64			id;	  } && PERF_SAMPLE_ID
	 *	{ u64			stream_id;} && PERF_SAMPLE_STREAM_ID
	 *	{ u32			cpu, res; } && PERF_SAMPLE_CPU
	 *	{ u64			period;   } && PERF_SAMPLE_PERIOD
	 *
	 *	{ struct read_format	values;	  } && PERF_SAMPLE_READ
	 *
	 *	{ u64			nr,
	 *	  u64			ips[nr];  } && PERF_SAMPLE_CALLCHAIN
	 *
	 *	#
	 *	# The RAW record below is opaque data wrt the ABI
	 *	#
	 *	# That is, the ABI doesn't make any promises wrt to
	 *	# the stability of its content, it may vary depending
	 *	# on event, hardware, kernel version and phase of
	 *	# the moon.
	 *	#
	 *	# In other words, PERF_SAMPLE_RAW contents are not an ABI.
	 *	#
	 *
	 *	{ u32			size;
	 *	  char                  data[size];}&& PERF_SAMPLE_RAW
	 *
	 *	{ u64                   nr;
	 *        { u64 from, to, flags } lbr[nr];} && PERF_SAMPLE_BRANCH_STACK
	 *
	 * 	{ u64			abi; # enum perf_sample_regs_abi
	 * 	  u64			regs[weight(mask)]; } && PERF_SAMPLE_REGS_USER
	 *
	 * 	{ u64			size;
	 * 	  char			data[size];
	 * 	  u64			dyn_size; } && PERF_SAMPLE_STACK_USER
	 *
	 *	{ u64			weight;   } && PERF_SAMPLE_WEIGHT
	 *	{ u64			data_src; } && PERF_SAMPLE_DATA_SRC
	 *	{ u64			transaction; } && PERF_SAMPLE_TRANSACTION
	 * };
	 */
	PERF_RECORD_SAMPLE			= 9,

	/*
	 * The MMAP2 records are an augmented version of MMAP, they add
	 * maj, min, ino numbers to be used to uniquely identify each mapping
	 *
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	u32				pid, tid;
	 *	u64				addr;
	 *	u64				len;
	 *	u64				pgoff;
	 *	u32				maj;
	 *	u32				min;
	 *	u64				ino;
	 *	u64				ino_generation;
	 *	u32				prot, flags;
	 *	char				filename[];
	 * 	struct sample_id		sample_id;
	 * };
	 */
	PERF_RECORD_MMAP2			= 10,

	PERF_RECORD_MAX,			/* non-ABI */
};

// Use first attribute for deciding if this is present, not the header!
// Why the first?!? idiots ...
struct PerfSampleId {
    static const size_t s_duplicateIdFromBack = 8;

    PerfSampleId(quint64 sampleType = 0) : pid(0), tid(0), time(0), id(0), streamId(0), cpu(0),
        res(0), m_sampleType(sampleType) {}
    quint32 pid;
    quint32 tid;
    quint64 time;
    quint64 id;
    quint64 streamId;
    quint32 cpu;
    quint32 res;

    quint64 length() const;
    quint64 sampleType() const { return m_sampleType; }

private:
    union {
        quint64 m_ignoredDuplicateId; // In the file format this is the same as id above
        quint64 m_sampleType; // As the id is ignored we can reuse the space for saving the flags
    };

    friend QDataStream &operator>>(QDataStream &stream, PerfSampleId &sampleId);
};

QDataStream &operator>>(QDataStream &stream, PerfSampleId &sampleId);

class PerfRecord {
public:
    quint32 pid() const { return m_sampleId.pid; }
    quint32 tid() const { return m_sampleId.tid; }

protected:
    PerfRecord(const PerfEventHeader *header, quint64 sampleType);
    PerfEventHeader m_header;
    PerfSampleId m_sampleId;
};

class PerfRecordMmap : public PerfRecord {
public:
    PerfRecordMmap(PerfEventHeader *header = 0, quint64 sampleType = 0);
private:
    quint32	m_pid;
    quint32 m_tid;
    quint64 m_addr;
    quint64 m_len;
    quint64 m_pgoff;
    QByteArray m_filename;

    friend QDataStream &operator>>(QDataStream &stream, PerfRecordMmap &record);
};

QDataStream &operator>>(QDataStream &stream, PerfRecordMmap &record);


class PerfRecordLost : public PerfRecord {
public:
    PerfRecordLost(PerfEventHeader *header = 0, quint64 sampleType = 0);
private:
    quint64 m_id;
    quint64 m_lost;

    friend QDataStream &operator>>(QDataStream &stream, PerfRecordLost &record);
};

QDataStream &operator>>(QDataStream &stream, PerfRecordLost &record);

class PerfRecordComm : public PerfRecord {
public:
    PerfRecordComm(PerfEventHeader *header = 0, quint64 sampleType = 0);
private:
    quint32 m_pid;
    quint32 m_tid;
    QByteArray m_comm;

    friend QDataStream &operator>>(QDataStream &stream, PerfRecordComm &record);
};

QDataStream &operator>>(QDataStream &stream, PerfRecordComm &record);

class PerfRecordSample : public PerfRecord {
public:
    PerfRecordSample(const PerfEventHeader *header = 0, const PerfEventAttributes *attributes = 0);
    quint64 registerAbi() const { return m_registerAbi; }
    quint64 registerValue(uint reg) const { return m_registers[reg]; }
    const QList<quint64> &registers() const { return m_registers; }
    const QByteArray &userStack() const { return m_userStack; }

private:
    struct ReadFormat {
        quint64 value;
        quint64 id;
    };

    struct BranchEntry {
        quint64 from;
        quint64 to;
    };

    quint64 m_readFormat;
    quint64 m_registerMask;

    quint64 m_ip;
    quint64 m_addr;
    quint64 m_period;
    quint64 m_timeEnabled;
    quint64 m_timeRunning;

    quint64 m_registerAbi;
    quint64 m_weight;
    quint64 m_dataSrc;
    quint64 m_transaction;

    QList<ReadFormat> m_readFormats;
    QList<quint64> m_callchain;
    QByteArray m_rawData;
    QList<BranchEntry> m_branchStack;
    QList<quint64> m_registers;
    QByteArray m_userStack;

    friend QDataStream &operator>>(QDataStream &stream, PerfRecordSample &record);
};

QDataStream &operator>>(QDataStream &stream, PerfRecordSample &record);

class PerfData
{
public:
    PerfData();
    bool read(QIODevice *device, const PerfHeader *header, const PerfAttributes *attributes,
              const PerfFeatures *features);

    const QList<PerfRecordSample> &sampleRecords() { return m_sampleRecords; }
    const QList<PerfRecordMmap> &mmapRecords() { return m_mmapRecords; }
private:
    QList<PerfRecordMmap> m_mmapRecords;
    QList<PerfRecordComm> m_commRecords;
    QList<PerfRecordLost> m_lostRecords;
    QList<PerfRecordSample> m_sampleRecords;
};

#endif // PERFDATA_H
