//
// Created by mck on 24-6-13.
//

#ifndef NUMERIC_H
#define NUMERIC_H



// 主函数：将中文数字字符串转换为float
inline float ConvertChineseNumberToFloat(std::string str)
{
    static std::unordered_map<std::string, int> CHINESE_TO_ARABIC = {
        {"零", 0}, {"一", 1}, {"二", 2}, {"三", 3}, {"四", 4},
        {"五", 5}, {"六", 6}, {"七", 7}, {"八", 8}, {"九", 9},
        {"十", 10}, {"百", 100}, {"千", 1000}, {"万", 10000}, {"亿", 100000000}
    };
    const static auto HANZI_SIZE = std::string("我").size();
    // 如果是十开头的数字，那么在前面加一个一
    if (str.substr(0, HANZI_SIZE) == "十")
    {
        str = "一" + str;
    }
    float result = 0;
    auto lastFactor = 0;
    auto maxFactor = 0;
    auto curSeg = 0;

    for (int i = 0; i < str.size(); i += HANZI_SIZE)
    {
        const auto curHanzi = str.substr(i, HANZI_SIZE);
        assert(CHINESE_TO_ARABIC.count(curHanzi));
        auto n = CHINESE_TO_ARABIC[curHanzi];
        // 遇到单位，计算之前的数值并重置乘数
        if (n >= 10)
        {
            // 单位创新高，那么将result和当前段相加并清空result
            if (n > maxFactor)
            {
                curSeg = (result + curSeg) * n;
                result = 0;
                maxFactor = n;
            }
            else
            {
                curSeg = curSeg * n;
                lastFactor = n;
            }
        }
        // 遇到数字
        else
        {
            if (lastFactor == 10)
            {
                curSeg += n;
            }
            else
            {
                result += curSeg;
                curSeg = n;
            }
        }
    }
    result += curSeg;

    return result;
}


// 将给定的int转换为中文数字字符串
inline std::string ConvertIntToChineseNumber(int n)
{
    const std::vector<std::string> base = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九"};
    std::string result;
    if (n < 0)
    {
        result += "负";
        n = -n;
    }
    if (n < 10)
    {
        result += base[n];
    }
    else
    {
        std::vector<int> factors = {100000000, 10000, 1000, 100, 10, 1};
        std::vector<std::string> units = {"亿", "万", "千", "百", "十", ""};
        for (int i = 0; i < factors.size(); i++)
        {
            int factor = factors[i];
            if (n >= factor)
            {
                int m = n / factor;
                n = n % factor;
                if (m > 1 || factor == 1)
                {
                    result += ConvertIntToChineseNumber(m);
                }
                result += units[i];
            }
        }
    }
    return result;
}

// 将给定的float转换为中文数字字符串
inline std::string ConvertFloatToChineseNumber(float f)
{
    std::string result;
    if (f < 0)
    {
        result += "负";
        f = -f;
    }
    int n = f;
    int m = f * 10 - n * 10;
    if (n == 0)
    {
        result += "零";
    }
    else
    {
        result += ConvertIntToChineseNumber(n);
    }
    if (m != 0)
    {
        result += "点";
        result += ConvertIntToChineseNumber(m);
    }
    return result;
}




#endif //NUMERIC_H
