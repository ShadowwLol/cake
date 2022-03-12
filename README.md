<div id="top"></div>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]


<h3 align="center">cake</h3>

  <p align="center">
    The fast crossplatform programming language
    <br />
    <a href="https://shadowwlol.github.io/cake/docs.html"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/ShadowwLol/cake">View Page</a>
    ·
    <a href="https://github.com/ShadowwLol/cake/issues">Report Bug</a>
    ·
    <a href="https://github.com/ShadowwLol/cake/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#building">Building</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
[![cake](cake.png)](https://github.com/ShadowwLol/cake)

*cake* is a fast crossplatform programming language able to be both interpreted and compiled to a native executable.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

The building of the interpreter is crossplatform **however**, the script is bash specific, replicate the commands if building on Windows or download [the latest build](https://github.com/ShadowwLol/cake/releases).

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* gcc / i686-w64-mingw32

Debian/Ubuntu:
  ```sh
  sudo apt-get install gcc
  ```

Archlinux/Manjaro:
  ```sh
  sudo pacman -S gcc
  ```

### Building

  ```sh
  $ git clone git@github.com:ShadowwLol/cake.git
  $ cd cake
  $ chmod +x build.sh
  $ ./build.sh - <linux/windows>
  ```

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

run the following to know more:
```sh
  $ ./cake --help
```

_For code examples, please refer to the [Documentation](https://shadowwlol.github.io/cake/docs.html)_

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [ ] Variables
- [ ] Comparison
- [ ] Loops
- [ ] Bitwise operators
- [ ] Preprocessors
- [ ] if var == str
- [ ] Less allocations
- [ ] Register-based bytecode
- [ ] Compilation to native executable

See the [open issues](https://github.com/ShadowwLol/cake/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

ShadowwLol - [@Shadowwlol](https://twitter.com/Shadowwlol) - shadowwlol613@protonmail.com

Project Link: [https://github.com/ShadowwLol/cake](https://github.com/ShadowwLol/cake)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Logo Designer](https://twitter.com/gu1_hyp3r)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/ShadowwLol/cake.svg?style=for-the-badge
[contributors-url]: https://github.com/ShadowwLol/cake/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/ShadowwLol/cake.svg?style=for-the-badge
[forks-url]: https://github.com/ShadowwLol/cake/network/members
[stars-shield]: https://img.shields.io/github/stars/ShadowwLol/cake.svg?style=for-the-badge
[stars-url]: https://github.com/ShadowwLol/cake/stargazers
[issues-shield]: https://img.shields.io/github/issues/ShadowwLol/cake.svg?style=for-the-badge
[issues-url]: https://github.com/ShadowwLol/cake/issues
[license-shield]: https://img.shields.io/github/license/ShadowwLol/cake.svg?style=for-the-badge
[license-url]: https://github.com/ShadowwLol/cake/blob/master/LICENSE.txt
[product-screenshot]: cake.png
