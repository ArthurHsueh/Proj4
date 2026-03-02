#include "StringUtils.h"
#include <cctype>
#include <algorithm>

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{ // Test case (end before start) FINISH MOD IMP!
    std::string result = str;
    if (end == 0){
        end = str.length();
    }

    if (start >= 0 && end >= 0){
        start = std::min((static_cast<ssize_t>(str.length())), start);
        ssize_t count = end - start;
        if (count >= 0){
            return result.substr(start, count); 
        }
        else{
            return "";
        }
    }
    
    else if (start >= 0 && end < 0){
        ssize_t count = (str.length() + end) - start;
        if (count >= 0){
            return result.substr(start, count); 
        }
        else{
            return "";
        }
    }
    
    else if (start < 0 && end >= 0){
        start = std::max((-1 * static_cast<ssize_t>(str.length())), start); //If negative index too big.
        ssize_t count = end - (str.length() + start);
        if (count >= 0){
            return result.substr((str.length() + start), count); 
        }
        else{
            return "";
        }
    }
    
    else{ //(start < 0 && end < 0)
        ssize_t count = -1 * (start - end);
        if (count >= 0){
            return result.substr((str.length() + start), count); 
        }
        else{
            return "";
        }
    }
}

std::string Capitalize(const std::string &str) noexcept{
    std::string result = str;
    if (str.empty()){
        return result;
    }
    else {
        result[0] = toupper(result[0]);
        if (str.length() > 1){
            for (int i = 1; i < str.length(); i++){
                result[i] = tolower(result[i]);
            }
        }  
        return result;
    }
}

std::string Upper(const std::string &str) noexcept{
    std::string result = str;
    if (str.empty()){
        return result;
    }
    for (int i = 0; i < str.length(); i++){
        result[i] = toupper(str[i]);
    }
    return result;
}

std::string Lower(const std::string &str) noexcept{
    std::string result = str;
    if (str.empty()){
        return result;
    }
    for (int i = 0; i < str.length(); i++){
        result[i] = tolower(str[i]);
    }
    return result;
}

std::string LStrip(const std::string &str) noexcept{
    std::string result = str;
    if (str.empty()){
        return result;
    }
    int count = 0;
    int i = 0; 
    while (i < str.length() && isspace(str[i])){
        count++;
        i += 1;
    }
    return result.substr(count, result.length() - count);
}

std::string RStrip(const std::string &str) noexcept{
    std::string result = str;
    if (str.empty()){
        return result;
    }
    int count = 0;
    int i = str.length() - 1;
    while (i >= 0 && isspace(str[i])){
        count++;
        i -= 1;
    }
    return result.substr(0, result.length() - count);
}

std::string Strip(const std::string &str) noexcept{
    std::string result = str;
    return LStrip(RStrip(result));
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    if (width <= str.length()){
        return result;
    }
    int n = width - str.length();
    if ((n % 2) == 0){
        for (int i = 0; i < n/2; i++){
            result.insert(result.begin(), fill);
            result.insert(result.end(), fill);
        }
    }
    else{
        if (str.length() % 2 == 0){
            for (int i = 0; i < (n+1)/2; i++){
                result.insert(result.begin(), fill);
            }
            for (int i = 0; i < (n-1)/2; i++){
                result.insert(result.end(), fill);
            }
        }
        else{
            for (int i = 0; i < (n-1)/2; i++){
                result.insert(result.begin(), fill);
            }
            for (int i = 0; i < (n+1)/2; i++){
                result.insert(result.end(), fill);
            }
        }
    }
    return result;
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    if (width <= str.length()){
        return result;
    }
    else{
        int n = width - str.length(); 
        for (int i = 0; i < n; i++) 
        {
            result.insert(result.end(), fill);
        }
    }
    return result;
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    if (width <= str.length()){
        return result;
    }
    else{
        int n = width - str.length(); 
        for (int i = 0; i < n; i++) 
        {
            result.insert(result.begin(), fill);
        }
    }
    return result;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    std::string result = str;
    int i = 0;
    if (old.length() == 0)
    {
        for (int i = 0; i < 2*str.length() + 1; i+=2){
            result.insert(i, rep);
        }
    return result;
    }
    while (i < result.length())
    {
        int j = result.find(old, i);
        if (j != std::string::npos)
        {
            result.replace(j, old.length(), rep);
            i = j + rep.length();
        }
        else
        {
            return result;
        }
    }
    return result;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    std::vector<std::string> result = {};
    int i = 0;
    int j = 0;

    if (splt == ""){
        while (i < str.length()){
            if (isspace(str[i])){
                int k = i;
                while(isspace(str[i])){
                    i++;
                }
                if (str.substr(j, k-j) != ""){
                    result.push_back(str.substr(j, k-j));
                }
                j = i;
            }
            else{
                i++;
            }
        }
        if (str.substr(j) != ""){
            result.push_back(str.substr(j));
        }
        return result;
    }
    
    while (i < str.length())
    {
        j = str.find(splt, i);
        if (j != std::string::npos)
        {
            result.push_back(str.substr(i, j - i)); 
            i = j + splt.length();
        }
        else
        {
            break;
        }
    }
    result.push_back(str.substr(i));
    return result;
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    if (vect.empty()){
        return "";
    }
    std::string result = "";
    int i = 0;
    for (int j = 0; j < vect.size(); j++){
        result.insert(i, vect[j]);
        i += vect[j].length();
        if (j < vect.size() - 1){
            result.insert(i, str); // Don't add &s at end
            i += str.length();
        }
    }
    return result;
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{ //Code inspired from: https://stackoverflow.com/questions/34546171/python-expandtabs-string-operation
    std::string result = "";
    int pos = 0;
    
    for(char c: str){
        if (c == '\t'){
            if (tabsize > 0){
                result.append((tabsize - pos % tabsize), ' ');
                pos = 0;
                continue;
            }
        }
        else if (c == '\n'){
            pos = 0;
            result += c;
        }
        else{
            pos += 1;
            result += c;
        }
    }
    return result;
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    int result = 0; //False: Consider capitalize True: Ignore case
    if (left.empty()){
        return right.length();
    }
    if (right.empty()){
        return left.length();
    }

    std::string l = left;
    std::string r = right;
    if (ignorecase){
        l = Lower(l);
        r = Lower(r);
    }

    std::string lhead = Slice(l, 0, 1);
    std::string rhead = Slice(r, 0, 1);;
    std::string ltail = Slice(l, 1, l.length());
    std::string rtail = Slice(r, 1, r.length());
    
    if (lhead == rhead){
        return EditDistance(ltail, rtail);
    }
    else{
        return (1 + std::min({EditDistance(ltail, right), EditDistance(left, rtail), EditDistance(ltail, rtail)}));
    }
}
};