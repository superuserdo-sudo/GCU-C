# Run: source ./env.sh
_GCU_ROOT=$(CDPATH= cd -- "$(dirname -- "${BASH_SOURCE:-$0}")" && pwd)
_GCU_BIN="$_GCU_ROOT/build/bin"
case ":$PATH:" in
    *":$_GCU_BIN:"*) ;;
    *) PATH="$_GCU_BIN:$PATH" ;;
esac
export PATH
unset _GCU_BIN _GCU_ROOT
