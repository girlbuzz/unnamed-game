let
  pkgs = import <nixpkgs> {};
in
  pkgs.mkShell {
    packages = with pkgs; [
      clang
      lldb
      clang-tools
      jq
    ];

    env.CC = "clang";
  }
