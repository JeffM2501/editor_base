#pragma once

#include <vector>
#include <memory>

class CancelationToken
{
public:
    inline void Release() { Active = false; }
    inline bool Valid()const  { return Active; }

    using Ptr = std::shared_ptr<CancelationToken>;
protected: 
    bool Active = true;
};

class CancelationTokenHost
{
public:
    CancelationTokenHost()
    {
        TokenPtr = std::make_shared<CancelationToken>();
    }

    virtual ~CancelationTokenHost()
    {
        TokenPtr->Release();
    }

    CancelationToken::Ptr Get() const { return TokenPtr; }

private:
    CancelationToken::Ptr TokenPtr;
};

template<typename F>
class EventHandler
{
private:
    std::vector<std::pair<F,CancelationToken::Ptr>> Callbacks;

public:
    inline void Add(F callback, CancelationToken::Ptr token = nullptr)
    {
        Callbacks.push_back(std::pair<F, CancelationToken::Ptr>{callback, token});
    }

    inline void Add(F callback, const CancelationTokenHost& token)
    {
        Callbacks.push_back(std::pair<F, CancelationToken::Ptr>{callback, token.Get()});
    }

    inline void Remove(F callback)
    {
        std::vector<::pair<F, CancelationToken::Ptr>>::iterator itr = Callbacks.begin();
        while (itr != Callbacks.end())
        {
            if (itr->first == callback)
                itr = Callbacks.erase(itr);
            else
                itr++;
        }
    }

    template<typename ...Args>
    inline void Call(Args&&... args)
    {
        std::vector<std::pair<F, CancelationToken::Ptr>>::iterator itr = Callbacks.begin();
        while (itr != Callbacks.end())
        {
            bool valid = !itr->second || itr->second->Valid();

            if (!valid)
            {
                itr = Callbacks.erase(itr);
            }
            else
            {
                itr->first(std::forward<Args>(args)...);
                itr++;
            }
        }
    }
};