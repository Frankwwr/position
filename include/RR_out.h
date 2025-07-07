// 本头文件是RS.h的重构，优化了绘图的方法改进了峰定位的函数。

#include "boardRR.C"

void SimplePause()
{
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool fileExists(const std::string &filename)
{
    return std::filesystem::exists(filename);
}

std::array<Double_t, 8> readFromFileToArray(const std::string &filename)
{
    std::array<Double_t, 8> arr;
    std::ifstream file(filename);

    if (file.is_open())
    {
        Double_t num;
        int i = 0;
        while (file >> num)
        {
            arr[i++] = num;
        }
        file.close();
    }
    else
    {
        std::cout << "Unable to open file: " << filename << std::endl;
    }
    return arr;
}

// 将数组中的数据写入文本文件
void writeFromArrayToFile(std::array<Double_t, 8> arr, const std::string &filename)
{
    std::ofstream file(filename, std::ofstream::trunc);

    if (file.is_open())
    {
        for (Double_t num : arr)
        {
            file << num << std::endl;
        }
        file.close();
    }
    else
    {
        std::cout << "Unable to open file: " << filename << std::endl;
    }
}

class rs
{
public:
    std::string name;
    std::string bfname;    // 标定所使用的文件，一般使用其本身
    std::string file_path; // 文件路径名称
    TCanvas *c1;
    TFile *file;
    int mark;

    std::array<Double_t, 8> hgpeak;
    std::array<Double_t, 8> zero_point;
    std::array<Double_t, 8> hgrate;
    std::array<Double_t, 8> hgzero;

    TH1F *lfithist;
    TH1F *hfithist;
    TH1F *fix_fithist;

    rs(int mark_t, std::string fname_t, std::string file_path_t = "", bool op_flag = false);
    void save(std::string savename);

    std::array<double, 2> foundpeak_dz(TH1F *hist);
    std::array<double, 2> hlresultc(TH2F *hist2d_lg, bool draw_flage = false);

    std::array<std::array<Double_t, 8>, 2> gethgpeak(DataContainer_all data); // include zero peak
    std::array<std::array<Double_t, 8>, 2> gethgrate(DataContainer_all data);

    void init(bool anti_flag); // 对于一块新板子的初始化函数
    ~rs();
};

std::array<double, 2> rs::foundpeak_dz(TH1F *hist) // 寻峰不稳定，不建议直接使用
{
    TSpectrum *T = new TSpectrum(200);
    TH1 *h_bg = T->Background(hist, 20, "same");
    hist->Add(h_bg, -1);
    int nfound = T->Search(hist); // sigma=3, threshold=3%
    Double_t *xpeaks = T->GetPositionX();
    if (nfound < 5)
        return {xpeaks[0], 500};
    std::sort(xpeaks, xpeaks + nfound);
    std::vector<double> arrx, arry;
    for (int i = 0; i < nfound - 4; i++)
    {
        arrx.push_back(i);
        arry.push_back(xpeaks[i + 4]);
    }
    bool flage = true; // 循环标识
    TF1 func1("line", "[0]*x+[1]", 0, 20);
    func1.SetParameters(500, 4000);
    while (flage)
    {
        int narr = arry.size();
        TGraph *graph = new TGraph(narr, arrx.data(), arry.data());
        graph->Fit(&func1, "Q");
        flage = false;
        arrx.clear();
        if (arry.size() < 10)
            break;
        for (int i = 0; i < arry.size(); i++)
        {
            if (abs(arry.at(i) - func1.Eval(i)) > 500)
            {
                flage = true;
                arry.erase(arry.begin() + i); // 删除后，i 不递增
            }
            else
            {
                arrx.push_back(i); // 使用当前 i
                i++;               // 仅在保留元素时递增
            }
        }
    }
    return {xpeaks[0], func1.GetParameter(0)};
}

std::array<std::array<Double_t, 8>, 2> rs::gethgpeak(DataContainer_all data)
{
    std::cout << "getdata" << std::endl;
    std::array<std::array<Double_t, 8>, 2> result;
    TCanvas *c[8];
    TH1F *hist[8];
    for (int i = 0; i < 8; i++)
    {
        hist[i] = new TH1F(Form("h1_%d", i), "Title", 1200, 0, 60000);
        c[i] = new TCanvas(Form("c1_%d", i), "Title", 1600, 1200);
    }
    for (long j = 0; j < data.size(); j++)
        for (int i = 0; i < 8; i++)
            if (data[j][i][2])
                hist[i]->Fill(data[j][i][0]);
    for (int i = 0; i < 8; i++)
    {
        std::cout << "Hist " << i << " entries: " << hist[i]->GetEntries() << std::endl;
        std::array<double, 2> save = foundpeak_dz(hist[i]);
        result[0][i] = save[0];
        result[1][i] = save[1];
        if (true)
        {
            c[i]->cd();
            hist[i]->Draw();
            c[i]->Update();
        }
    }
    SimplePause();
    for (int i = 0; i < 8; i++)
    {
        delete hist[i];
        delete c[i];
    }
    return result;
}

std::array<double, 2> rs::hlresultc(TH2F *hist2d_lg, bool draw_flage = false) // 返回其增益的斜率
{
    TCanvas c("c1", "Title", 1600, 1200);
    int n = hist2d_lg->GetNbinsX();
    bool xmin_flag = false, xmax_flag = false;
    double xmin, xmax;
    auto x = new double[n];
    auto y = new double[n];
    int m = 0;
    for (size_t i = 0; i < n; i++)
    {
        long count = 0;
        double rate = 0;
        for (size_t j = 0; j < n; j++)
        {
            count += hist2d_lg->GetBinContent(i, j);
            rate += hist2d_lg->GetBinContent(i, j) * hist2d_lg->GetYaxis()->GetBinCenter(j);
        }
        if (count < 10)
        {
            if (xmin_flag && !xmax_flag && y[m - 1] > 30000)
            {
                xmax = x[m];
                xmax_flag = true;
            }
        }
        else
        {
            m++;
            x[m] = hist2d_lg->GetXaxis()->GetBinCenter(i);
            y[m] = rate / count;
            if ((!xmin_flag) && y[m] > 5000)
            {
                xmin = x[m];
                xmin_flag = true;
            }
        }
    }
    auto graph_fit = new TGraph(m, x, y);
    TF1 *f1 = new TF1("len", "[0]*x+[1]", xmin, xmax - 15000);
    graph_fit->Fit("len", "RQ", "", xmin, xmax - 15000);
    if (draw_flage)
    {
        c.cd();
        graph_fit->DrawClone();
        c.Update();
        SimplePause();
    }
    delete[] x;
    delete[] y;
    return {f1->GetParameter(0), f1->GetParameter(1)};
}

std::array<std::array<Double_t, 8>, 2> rs::gethgrate(DataContainer_all data)
{
    std::array<std::array<Double_t, 8>, 2> result;
    TH2F *hist2d[8];
    for (int i = 0; i < 8; i++)
        hist2d[i] = new TH2F(Form("h2d_%d", i), "Title", 100, 0, 60000, 100, 0, 60000);
    for (int j = 0; j < data.size(); j++)
        for (int i = 0; i < 8; i++)
            hist2d[i]->Fill(data[j][i][1], data[j][i][0]);
    for (int i = 0; i < 8; i++)
    {
        std::cout << "Hist2d " << i << " entries: " << hist2d[i]->GetEntries() << std::endl;
        std::array<double, 2> save = hlresultc(hist2d[i]);
        result[0][i] = save[0];
        result[1][i] = save[1];
    }
    return result;
}

void rs::init(bool anti_flag)
{
    auto hgpeak_file = file_path + "hgpeak.txt";
    auto zero_point_file = file_path + "zero_point.txt";
    auto hgrate_file = file_path + "hgrate.txt";
    auto hgzero_file = file_path + "hgzero.txt";

    if (!fileExists(bfname)) // 文件存在性检查
        std::cerr << "ERROR: File not found: " << bfname << std::endl;

    if (!fileExists(hgpeak_file) || !fileExists(zero_point_file) || !fileExists(hgrate_file) || !fileExists(hgzero_file)) // 这位置应该改为相对位置
    {
        TFile *getfile = TFile::Open(bfname.c_str());
        if (!getfile || getfile->IsZombie())
        {
            std::cerr << "ERROR: Failed to open file: " << bfname << std::endl;
            return;
        }

        std::cout << '3';
        TTree *tree;
        getfile->GetObject("board", tree);
        board b1(tree);

        // 翻转电子学版
        DataContainer_all data_all = anti_flag
                                         ? b1.Loop_all_anti()
                                         : b1.Loop_all();

        std::cout << data_all.size() << std::endl;
        std::array<std::array<Double_t, 8>, 2> result = gethgpeak(data_all);
        zero_point = result[0];
        hgpeak = result[1];
        std::cout << "peakalready" << std::endl;
        std::array<std::array<Double_t, 8>, 2> rate_resule = gethgrate(data_all);
        hgrate = rate_resule[0];
        hgzero = rate_resule[1];
        std::cout << "ratealready" << std::endl;
        writeFromArrayToFile(zero_point, zero_point_file);
        writeFromArrayToFile(hgpeak, hgpeak_file);
        writeFromArrayToFile(hgrate, hgrate_file);
        writeFromArrayToFile(hgzero, hgzero_file);

        if (getfile)
        {
            getfile->Close();
            delete getfile;
        }
    }
    else
    {
        hgpeak = readFromFileToArray(hgpeak_file);
        zero_point = readFromFileToArray(zero_point_file);
        hgrate = readFromFileToArray(hgrate_file);
        hgzero = readFromFileToArray(hgzero_file);
    }
}

rs::rs(int mark_t, std::string fname_t, std::string file_path_t, bool op_flag)
    : mark(mark_t), name(file_path_t + fname_t), bfname(file_path_t + fname_t), file_path(file_path_t) // 初始函数
{
    init(op_flag);
    file = TFile::Open(name.c_str());
    TTree *tree;
    file->GetObject("board", tree);
    board b1(tree);

    // 翻转电子学版
    DataContainer data = op_flag
                             ? b1.Loop_single_anti(mark + 8)
                             : b1.Loop_single(mark + 8);

    std::cout << "get_rate_already" << std::endl;

    lfithist = new TH1F(Form("lgfit_%d", mark), Form("lgfit_%d", mark), 600, 0, 300);
    hfithist = new TH1F(Form("hgfit_%d", mark), Form("hgfit_%d", mark), 300, 0, 150);
    fix_fithist = new TH1F(Form("fgfit_%d", mark), Form("fgfit_%d", mark), 600, 0, 300);

    // 下面绘制图像
    c1 = new TCanvas(Form("c_%d", mark), "c", 1000, 700);

    for (const auto each : data) // HG:LG;:TDC
    {
        if (each[2])
        {
            lfithist->Fill((each[1] * hgrate[mark] + hgzero[mark] - zero_point[mark]) / hgpeak[mark]);
            hfithist->Fill((each[0] - zero_point[mark]) / hgpeak[mark]);
            if (each[1] > 15000)
                fix_fithist->Fill((each[1] * hgrate[mark] + hgzero[mark] - zero_point[mark]) / hgpeak[mark]);
            else
                fix_fithist->Fill((each[0] - zero_point[mark]) / hgpeak[mark]);
        }
    }
}

// 在 rs 类定义的公有或私有部分添加析构函数实现（根据声明位置）
rs::~rs()
{
    delete lfithist;    // 释放低增益直方图
    delete hfithist;    // 释放高增益直方图
    delete fix_fithist; // 释放修正直方图
    delete c1;
    if (file)
    {
        file->Close();
        delete file;
    }
}

double arcgaussianFunction(double *var, double *par)
{
    return par[1] + par[2] * TMath::Sqrt(-2 * TMath::Log(var[0] / par[0])); // 高斯反函数
}

double fit_lgth2(int way, bool high_fit_flag = false, bool fix_fit_flag = false)
{
    auto *f = new rs(way, "Board0-Aligned.root");
    TH1F *hist = f->lfithist;
    int zero_point = 20;
    if (high_fit_flag)
        hist = f->hfithist;
    if (fix_fit_flag)
        hist = f->fix_fithist;
    int mean = hist->GetMean();
    double bestChi2 = 1e9; // 初始化为一个很大的值
    double bestSegmentPoint = 20;
    for (int delta = 0; delta < 50; delta++)
    {
        TF1 *fit1 = new TF1("linearFit", "pol0", zero_point, mean + delta);
        hist->Fit(fit1, "RQ");
        // 拟合后半部分数据
        TF1 *fit2 = new TF1("gaussianFit", "gaus", mean + delta, 300);
        hist->Fit(fit2, "RQ");
        double chi2 = 0.3 * fit1->GetChisquare() + fit2->GetChisquare();
        if (chi2 < bestChi2)
        {
            bestChi2 = chi2;
            bestSegmentPoint = mean + delta; // 保存分段点
        }
        delete fit1; // 删除拟合函数，避免内存泄漏
        delete fit2;
    }
    TF1 *fit1 = new TF1("linearFit", "pol0", zero_point, bestSegmentPoint);
    hist->Fit(fit1, "RQ");
    TF1 *fit2 = new TF1("gaussianFit", "gaus", bestSegmentPoint, 300);
    hist->Fit(fit2, "RQ");
    hist->Draw();
    fit1->Draw("Same");
    fit2->Draw("Same");
    auto par_list = fit2->GetParameters();
    double result = par_list[1] + par_list[2] * TMath::Sqrt(-2 * TMath::Log(fit1->GetParameter(0) * 0.5 / par_list[0]));
    std::stringstream s_out, s_mark;
    s_out << "result:" << result;
    TLatex *latex = new TLatex(0.6, 0.6, s_out.str().c_str());
    latex->SetNDC();
    latex->Draw("Same");
    s_mark << "canvas" << way << ".png";
    // gPad->GetCanvas()->SaveAs(s_mark.str().c_str(), "RECREATE");
    // delete f;
    return result;
}
