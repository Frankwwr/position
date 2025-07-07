#include <TSystem.h>
#include <TString.h>
#include <iostream>

#ifdef WSL_SYS

void moveFile(const TString& source, const TString& target) {
    // 获取当前工作目录
    TString cwd = gSystem->WorkingDirectory();
    std::cout << "当前工作目录: " << cwd << std::endl;
    
    // 检查源文件是否存在
    if (gSystem->AccessPathName(source)) {
        std::cerr << "错误: 源文件不存在: " << source << "\n";
        return;
    }
    std::cout << "源文件存在: " << source << std::endl;

    // 统一路径分隔符为Windows风格
    TString final_target = target;
    final_target.ReplaceAll("/", "/");  // 使用Windows路径分隔符
    
    // 检查目标路径是否是目录
    FileStat_t fileStat;
    bool isDirectory = false;
    
    if (gSystem->GetPathInfo(final_target, fileStat) == 0) {
        isDirectory = R_ISDIR(fileStat.fMode);
        std::cout << "目标路径存在，类型: " << (isDirectory ? "目录" : "文件") << std::endl;
    } else {
        // 如果目标路径以反斜杠结尾，则视为目录
        isDirectory = final_target.EndsWith("/");
        std::cout << "目标路径不存在，视为: " << (isDirectory ? "目录" : "文件") << std::endl;
    }
    
    // 如果是目录，则追加源文件名
    if (isDirectory) {
        if (!final_target.EndsWith("/")) {
            final_target += "/";
        }
        final_target += gSystem->BaseName(source);
    }
    std::cout << "最终目标路径: " << final_target << std::endl;

    // 创建目标目录
    TString target_dir = gSystem->DirName(final_target);
    std::cout << "需要创建的目录: " << target_dir << std::endl;
    
    if (gSystem->AccessPathName(target_dir)) {
        std::cout << "创建目录: " << target_dir << std::endl;
        if (gSystem->mkdir(target_dir, kTRUE)) {  // kTRUE表示递归创建
            std::cerr << "错误: 无法创建目录: " << target_dir << "\n";
            return;
        }
    } else {
        std::cout << "目录已存在: " << target_dir << std::endl;
    }

    // 直接移动文件（假设在同一设备）
    if (gSystem->Rename(source, final_target) == 0) {
        std::cout << "文件移动成功: " << source << " -> " << final_target << "\n";
    } else {
        std::cerr << "错误: 文件移动失败\n";
    }
    
    // 验证移动结果
    if (gSystem->AccessPathName(final_target)) {
        std::cerr << "错误: 移动后目标文件不存在!\n";
    } else {
        std::cout << "验证: 目标文件存在: " << final_target << std::endl;
    }
}

#endif