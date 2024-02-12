let
  pkgs = import <nixpkgs> {};
in
  pkgs.mkShell {
    packages = with pkgs; [
      clang
      lldb
      clang-tools
    ];
  }
