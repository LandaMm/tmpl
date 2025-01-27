# Templify (tmpl) 🚀

Templify is a **CLI tool** and **scripting language** designed to help developers and anyone working with files and projects streamline their workflow. It allows users to record, create, publish, and reuse development patterns, making coding faster, easier, and more efficient. Say goodbye to repetitive tasks and wasted time!

## Flow

The first step is to write tmpl script itself.

Here is an example:

```
// HelloWorld.tmpl
// prints message to stdout

->main {
    @arg string message;

    print(message);
}
```

After you wrote your tmpl script you create a folder in project root called `.tmpl`.

```
.
├── rest
│   ├── get_password.http
│   ├── get_single_password.http
│   ├── save_password.http
│   └── setup.http
├── src
│   ├── bin
│   │   └── configure.rs
│   ├── db.rs
│   ├── main.rs
│   ├── middleware.rs
│   ├── models.rs
│   ├── routes.rs
│   ├── schema.rs
│   ├── server.rs
│   ├── service.rs
│   └── settings.rs
├── .tmpl                           < -- folder you should create
├── Cargo.lock
├── Cargo.toml
├── config.yml
├── diesel.toml
├── docker-compose.yml
└── README.md

```

Now put your script inside that `.tmpl` folder.

After that you can run your script inside your project using

```sh
$ tmpl HelloWorld --message "Hi, everyone!"
```

Expected Output:

```
Hi everyone
```

### Defining Subcommand (Procedure)

Update `HelloWorld.tmpl` by adding new procedure:

```
// HelloWorld.tmpl
// prints message to stdout

->main {
    @arg string message;

    print(message);
}

// says bye to stdout

->bye {
    print("Bye!");
}
```

Now run that procedure using following command:

```sh
$ tmpl HelloWorld bye
```

Expected Output:

```
Bye!
```

## 🌱 Current Features (Development Phase)

Templify is still in its development phase, but it already includes a basic scripting language (`.tmpl`) that allows you to execute various instructions for automating tasks and managing projects. The following features are available:

- **File Creation** 📂: Automate the process of generating files.
- **File Path Management** 📁: Easily read, write, and manipulate file paths.
- **File Writing** 📝: Write content to files with ease.
- **Project Management** 🔧: Check for `package.json`, recognize project location and type, and more.
- **Dynamic Scripting** 🧑‍💻: Use `.tmpl` scripts to execute all these tasks with just a command.

## 🚀 Future Plans

Templify is still evolving, with exciting new features in the pipeline!

### 🌍 Public Repository & Pattern Sharing

In the near future, Templify will have its own **public repository** where users can **publish** and **download** custom development patterns. This will allow the community to share their work and contribute to a collective library of reusable templates, empowering developers worldwide.

### 🔄 CLI Recorder

One of the most exciting long-term goals is the creation of a **CLI recorder** that will track changes in a project and automatically generate `.tmpl` scripts based on those actions. Imagine a tool that records your actions, such as file creations, edits, and deletions, and creates reusable scripts for you. This will save you from repeating the same tasks over and over again and boost your productivity!

## 💻 Technology Stack

- **Programming Language**: C++ 💡
- **Primary Platform**: Windows (Developed in Visual Studio 2022) 🖥️
- **Cross-Platform**: Planned for future versions 🌐

Templify is being built with **future scalability** in mind, and we aim to make it cross-platform, so it can be used by developers on any operating system.

## 🙌 Contribution

We welcome contributions from anyone who shares the vision of making development easier, faster, and more efficient. Whether you're a seasoned developer or just starting out, you can help make Templify better.

Here’s how you can contribute:

- **Fork the Repository** 🔨
- **Submit Pull Requests** 🔁
- **Report Issues** 🚨
- **Suggest Features** 💡

If you need any help or have questions, feel free to reach out to me at:

📧 **Email**: [qwer.009771@gmail.com](mailto:qwer.009771@gmail.com)

Looking forward to hearing from you!

## 📜 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

### 📅 Roadmap

- **Q1 2025**: Release core scripting features
- **Q2 2025**: Implement public repository for pattern sharing
- **Q4 2025**: Release CLI recorder tool
