#ifndef USER_HPP
#define USER_HPP

# include "include.hpp"

class User
{
private:
	std::string _nickname;
	std::string	_username;
	std::string _realname;

	int		_rights;
	std::vector<std::string> _channels;

	std::string _tmpPassword;
	std::string _tmpRequest;

	std::string	_operName;
	bool		_isRegistered;

public:
	~User();
	User();
	User(User const & other);
	User & operator=(User const & other);

	void	setTmpPwd(std::string tmpPwd);
	void	setNickname(std::string nickname);
	void	setUsername(std::string username);
	void	setRealname(std::string realname);
	void	setOperName(std::string realname);
	void	appendTmpRequest(std::string request);

	std::string & getTmpPwd() ;
	std::string const & getUsername() const;
	std::string const & getRealName() const;
	std::string const & getNickname() const;
	bool const & getRegistered() const;
	std::string & getTmpRequest() ;
	std::string const & getOperName() const;
	
	bool	isRegistered();
	void	setRegistered(int val);
	void	cleanTmpRequest();
};

#endif