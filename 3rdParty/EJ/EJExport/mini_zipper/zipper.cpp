#include "zipper.h"
#include "qzipwriter_p.h"

#include <qtiocompressor.h>
#include <fstream>
#include <QDebug>
#include <QDataStream>
#include <QBuffer>
#include <QDirIterator>


bool Zipper::toZip(QString path_files, QString const& zip_path) const {
    char snm = '/';
    if (path_files[path_files.size() - 1] != snm) {
        path_files.append(snm);
    }
    QZipWriter zipWriter(zip_path);
    zipWriter.setCompressionPolicy(QZipWriter::AlwaysCompress);
    QDirIterator iterator(path_files, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QString file = iterator.next();
        if(iterator.fileInfo().isDir()) {
            zipWriter.addDirectory(file.remove(path_files));
        } else if(iterator.fileInfo().isFile())
        {
            QFile writableFile(file);
            if(!writableFile.open(QFile::ReadOnly)) continue;
            zipWriter.addFile(file.remove(path_files), writableFile.readAll());
        }
    }
    zipWriter.close();
    return true;
}

bool Zipper::unzip(QString path_files, QString const& zip_path) const {
    char snm = '/';
    if (path_files[path_files.size()-1] != snm) {
        path_files.append(snm);
    }
    QDir dir;
    dir.mkpath(path_files);
    QFile file(zip_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
        return false;
    }

    quint32 signature = 0;
    quint32 comp_size, uncomp_size;
    quint16 comp_method, len_name, len_extra;
    quint32 crc, ext_at, relative_offset, size_dir, start_cen;
    quint16 extract_version, made_version, bit_flag, time, date, num_dis, int_at, disk_con, num_cen, num_total, len_com;

    bool result = true;
    QDataStream s(&file);
    s.setByteOrder(QDataStream::LittleEndian);
    while (!file.atEnd() && result) {
        if (0 == signature) {
            s >> signature;
        }
        if (0x04034b50 == signature) {
            signature = 0;
            s >> extract_version >> bit_flag >> comp_method >> time >> date >> crc >> comp_size >> uncomp_size >> len_name >> len_extra;
            const QByteArray file_name = file.read(len_name);
            file.read(len_extra);
            if(snm == file_name[file_name.size() - 1]) {
                QString p = path_files + file_name;
                if(!(result = dir.exists(p) || dir.mkpath(p))) {
                        break;
                }
            } else {
                int index = -1;
                for (int i = file_name.size() - 1; i >= 0 && index < 0; --i){
                    if (file_name[i] == snm) {
                        index = i;
                    }
                }
                dir.mkpath(path_files + file_name.left(index));
                QFile fw(path_files + file_name);
                QByteArray comp_data;
                if ((bit_flag & 0xff) != 8) {
                    comp_data = file.read(comp_size);
                } else {
                    bool read = true;
                    quint32 state = 0;
                    while (read) {
                        unsigned char ch;
                        s >> ch;
                        comp_data.append(char(ch));
                        if (0 == state && ch == 0x50) {
                            state = 1;
                        } else if (1 == state && ch == 0x4b) {
                            state = 2;
                        } else if (2 == state) {
                            state = ch;
                        } else if ((0x07 == state && 0x08 == ch) ||
                                   (0x03 == state && 0x04 == ch) ||
                                   (0x01 == state && 0x02 == ch) ||
                                   (0x05 == state && 0x06 == ch)) {
                            signature = (quint32(ch) << 24) | (state << 16) | 0x4b50;
                            state = 0;
                            read = false;
                        } else {
                            state = 0;
                        }
                    }
                    comp_data.remove(comp_data.size()-4, 4);
                    if (0x08074b50 == signature) {
                        signature = 0;
                        s >> crc >> comp_size >> uncomp_size;
                    } else {
                        comp_data.remove(comp_data.size()-12, 12);
                    }
                }
                QByteArray uncomp_data;
                if (comp_method == 0) {
                    uncomp_data = comp_data;
                } else {
                    QBuffer compBuf(&comp_data);
                    QtIOCompressor compressor(&compBuf);
                    compressor.setStreamFormat(QtIOCompressor::RawZipFormat);
                    compressor.open(QIODevice::ReadOnly);
                    uncomp_data = compressor.readAll();
                }

                if ((result = fw.open(QIODevice::WriteOnly))) {
                    result = fw.write(uncomp_data) != -1;
                    fw.close();
                }
            }
        } else if (0x02014b50 == signature) {
            signature = 0;
            s >> made_version >> extract_version >> bit_flag >> comp_method >> time >> date >> crc >> comp_size >>
                    uncomp_size >> len_name >> len_extra >> len_com >> num_dis >> int_at >> ext_at >> relative_offset;
            file.read(len_name);
            file.read(len_extra);
            file.read(len_com);
        } else if (0x06054b50 == signature) {
            signature = 0;
            s >> num_dis >> disk_con >> num_cen >> num_total >> size_dir >> start_cen >> len_com;
            file.read(len_com);
        } else {
            result = false;
        }
    }
    file.close();
    return result;
}
